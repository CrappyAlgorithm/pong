/* FreeRTOS.org includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "startup.h"
#include "wrap.h"
#include "display.h"
#include "framebuffer.h"
#include "pong.h"
#include "pong_field.h"

/*-----------------------------------------------------------*/


static void init_setup(void);
static void init_gpio(void);

void handle_trap_button();
void init_irq();

static uint8_t direction = LEFT;
/**
 * Defines the pin mapping for button usage.
 * The values match to the FE310. Be careful, the connection pins to the Red-V Board may differ.
 * Red-V-Pins: GREEN = 2, BLUE = 3, YELLOW = 4, RED = 5
 * 
 */
pin_mapping BUTTON[4] = {{GREEN, 18}, {BLUE, 19}, {YELLOW, 20}, {RED, 21}};


/* The task functions. */
void vTaskField( void *pvParameters );
void vTask2( void *pvParameters );
void vTask3( void *pvParameters );

const char *pcTextForTask1 = "Task 1\n";
const char *pcTextForTask2 = "Task 2\n";
const char *pcTextForTask3 = "Task 3\n";

void delay(uint32_t f_milliseconds) {
    volatile uint64_t *now = (volatile uint64_t*)(CLINT_CTRL_ADDR + CLINT_MTIME);
    volatile uint64_t then = *now + f_milliseconds*(RTC_FREQ / 2000);
    while (*now < then);
}

static void init_setup(void) {
	/* _init for uart printf */
	_init();
	
	init_gpio();
	init_irq();
	oled_init();
	init_pong();
	fb_init();
	delay(2000);
}

void init_gpio(void) {
	
    for (uint32_t i = 0; i < COLOR_COUNT; i++) {
        // init button
        REG32(GPIO_BASE + GPIO_IOF_EN) &= ~(1 << BUTTON[i].pin);
        REG32(GPIO_BASE + GPIO_PULLUP_EN) |= 1 << BUTTON[i].pin;
	    REG32(GPIO_BASE + GPIO_INPUT_EN) |= 1 << BUTTON[i].pin;
	    REG32(GPIO_BASE + GPIO_OUTPUT_EN) &= ~(1 << BUTTON[i].pin);
	    REG32(GPIO_BASE + GPIO_OUTPUT_VAL) &= ~(1 << BUTTON[i].pin);
    }
	
}

void handle_trap_button() {
	// claim interrupt
	uint32_t nb = REG32(PLIC_BASE + PLIC_CLAIM);

	for (int i = 0; i < COLOR_COUNT; i++) {
		if (nb == BUTTON[i].pin + 8) {
			printf("button %d pressed!\n", i);
			move_paddle(i);
			// clear gpio pending interrupt
			REG32(GPIO_BASE + GPIO_RISE_IP) |= (1 << BUTTON[i].pin);
		}
	}
	
	// complete interrupt
	REG32(PLIC_BASE + PLIC_CLAIM) = nb;
}

void init_irq() {
	// PLIC, 52 sources, 7 priorities, all off
	REG32(PLIC_BASE + PLIC_ENABLE) = 0;
	REG32(PLIC_BASE + PLIC_ENABLE + 4) = 0;
	REG32(PLIC_BASE + PLIC_THRESH) = 0;

	for (int i = 0; i < COLOR_COUNT; i++) {
		// enable irq for button and set priority for button to 1
		// interrupts for gpio start at 8
		REG32(PLIC_BASE + PLIC_ENABLE) |= (1 << (8 + BUTTON[i].pin));
		REG32(PLIC_BASE + 4 * (8 + BUTTON[i].pin)) = 1;

		// set handler will be handled by FreeRTOS

		// irq at rising
		REG32(GPIO_BASE + GPIO_RISE_IE) |= (1 << BUTTON[i].pin);

		// clear gpio pending interrupt
		REG32(GPIO_BASE + GPIO_RISE_IP) |= (1 << BUTTON[i].pin);
	}
	// set mie and mstatus will be handled by FreeRTOS
}

/*-----------------------------------------------------------*/
int main( void )
{
	init_setup();
	xTaskCreate( vTaskField, "Field", 1000, (void*)pcTextForTask1, 2, NULL );
	xTaskCreate( vTask2, "Task 2", 1000, (void*)pcTextForTask2, 1, NULL );
	//xTaskCreate( vTask3, "Task 3", 1000, (void*)pcTextForTask3, 1, NULL );

	/* start scheduler */
	vTaskStartScheduler();

	for( ;; );
	return 0;
}

/*-----------------------------------------------------------*/
void vTaskField( void *pvParameters )
{
	TickType_t xLastWakeTime;
	const TickType_t xDelay = pdMS_TO_TICKS( 10 );

	xLastWakeTime = xTaskGetTickCount();

	for( ;; ) {
		// printf("print field!\n");

		do_move();
		fb_flush();
		vTaskDelayUntil( &xLastWakeTime, xDelay );
	}
}

/*-----------------------------------------------------------*/
void vTask2( void *pvParameters )
{
	for( ;; )
	{
		printf(pcTextForTask2);
	}
}

/*-----------------------------------------------------------*/
void vTask3( void *pvParameters )
{
	for( ;; )
	{
		printf(pcTextForTask3);
	}
}
