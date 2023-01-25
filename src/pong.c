#include "FreeRTOS.h"
#include "task.h"
#include "startup.h"
#include "wrap.h"
#include "display.h"
#include "framebuffer.h"
#include "pong.h"
#include "pong_field.h"
#include "semphr.h"

/*-----------------------------------------------------------*/


static void init_setup(void);
static void init_gpio(void);

static void init_irq(void);
static void delay(uint64_t f_milliseconds);

static SemaphoreHandle_t framebuffer_mutex;
static SemaphoreHandle_t sem_interrupt;
static uint64_t interrupt_color;

/**
 * Defines the pin mapping for button usage.
 * The values match to the FE310. Be careful, the connection pins to the Red-V Board may differ.
 * Red-V-Pins: GREEN = 2, BLUE = 3, YELLOW = 4, RED = 5
 * 
 */
static pin_mapping BUTTON[4] = {{GREEN, 18}, {BLUE, 19}, {YELLOW, 20}, {RED, 21}};


/* The task functions. */
static void vTaskCore( void * pvParameters );
static void vTaskField( void * pvParameters );
static void vTaskScore( void * pvParameters );
static void vTaskButton( void * pvParameters );

static void delay(uint64_t f_milliseconds) {
	volatile uint64_t then = REG64(CLINT_CTRL_ADDR + (uint16_t)CLINT_MTIME) + (f_milliseconds * ((uint64_t)RTC_FREQ / 1000ULL));
	while (REG64(CLINT_CTRL_ADDR + (uint16_t)CLINT_MTIME) < then) {};
}

static void init_setup(void) {
	/* _init for uart printf */
	//_init();
	
	framebuffer_mutex = xSemaphoreCreateMutex();
	sem_interrupt = xSemaphoreCreateBinary();

	init_gpio();
	init_irq();
	oled_init();
	init_pong();
	fb_init();
	delay(2000);
}

static void init_gpio(void) {
	
    for (uint32_t i = 0; i < COLOR_COUNT; i++) {
        // init button
        REG32(GPIO_BASE + GPIO_IOF_EN) &= ~(1u << BUTTON[i].pin);
        REG32(GPIO_BASE + GPIO_PULLUP_EN) |= 1u << BUTTON[i].pin;
	    REG32(GPIO_BASE + GPIO_INPUT_EN) |= 1u << BUTTON[i].pin;
	    REG32(GPIO_BASE + GPIO_OUTPUT_EN) &= ~(1u << BUTTON[i].pin);
	    REG32(GPIO_BASE + GPIO_OUTPUT_VAL) &= ~(1u << BUTTON[i].pin);
    }
	
}

void handle_trap_button(void) {
	// claim interrupt
	uint32_t nb = REG32(PLIC_BASE + PLIC_CLAIM);
	for (uint16_t i = 0; i < COLOR_COUNT; i++) {
		if (nb == (BUTTON[i].pin + 8u)) {
			interrupt_color = i;
			// clear gpio pending interrupt
			REG32(GPIO_BASE + GPIO_RISE_IP) |= (1u << BUTTON[i].pin);
		}
	}
	// complete interrupt
	REG32(PLIC_BASE + PLIC_CLAIM) = nb;
	BaseType_t  xHigherPriorityTaskWoken = pdTRUE;
  	if (xSemaphoreGiveFromISR(sem_interrupt, &xHigherPriorityTaskWoken) != pdPASS) {}
}

static void init_irq(void) {
	// PLIC, 52 sources, 7 priorities, all off
	REG32(PLIC_BASE + PLIC_ENABLE) = 0;
	REG32(PLIC_BASE + PLIC_ENABLE + 4) = 0;
	REG32(PLIC_BASE + PLIC_THRESH) = 0;

	for (uint16_t i = 0; i < COLOR_COUNT; i++) {
		// enable irq for button and set priority for button to 1
		// interrupts for gpio start at 8
		REG32(PLIC_BASE + PLIC_ENABLE) |= (1u << (8u + BUTTON[i].pin));
		REG32((uint32_t)PLIC_BASE + 4u * (8u + BUTTON[i].pin)) = 1u;

		// set handler will be handled by FreeRTOS

		// irq at rising
		REG32(GPIO_BASE + GPIO_RISE_IE) |= (1u << BUTTON[i].pin);

		// clear gpio pending interrupt
		REG32(GPIO_BASE + GPIO_RISE_IP) |= (1u << BUTTON[i].pin);
	}
	// set mie and mstatus will be handled by FreeRTOS
}

/*-----------------------------------------------------------*/
int main( void )
{
	uint8_t error_task_creation = 0;
	init_setup();
	if (xTaskCreate( vTaskCore, "Core", 500, NULL, 3, NULL) != pdPASS) {
		error_task_creation = 1;
	}
	if (xTaskCreate( vTaskField, "Field", 500, NULL, 2, NULL) != pdPASS) {
		error_task_creation = 1;
	}
	if (xTaskCreate( vTaskScore, "Score", 500, NULL, 2, NULL) != pdPASS) {
		error_task_creation = 1;
	}
	if (xTaskCreate( vTaskButton, "Button", 500, NULL, 4, NULL) != pdPASS) {
		error_task_creation = 1;
	}

	if (error_task_creation == 1u) {
		return 1;
	}

	/* start scheduler */
	vTaskStartScheduler();

	for( ;; ) {};
	return 0;
}

/*-----------------------------------------------------------*/
static void vTaskCore( void * pvParameters )
{
	TickType_t xLastWakeTime;
	const TickType_t xDelay = pdMS_TO_TICKS( 10 );

	xLastWakeTime = xTaskGetTickCount();

	for( ;; ) {
		do_move();
		vTaskDelayUntil( &xLastWakeTime, xDelay );
	}
}

/*-----------------------------------------------------------*/
static void vTaskField( void * pvParameters )
{
	TickType_t xLastWakeTime;
	const TickType_t xDelay = pdMS_TO_TICKS( 10 );

	xLastWakeTime = xTaskGetTickCount();

	for( ;; ) {
		if (xSemaphoreTake(framebuffer_mutex, portMAX_DELAY) == pdTRUE) {
			write_field_to_framebuffer();
			fb_flush();
			if (xSemaphoreGive(framebuffer_mutex) == pdFALSE) {}
		}
		vTaskDelayUntil( &xLastWakeTime, xDelay );
	}
}

/*-----------------------------------------------------------*/
static void vTaskScore( void * pvParameters )
{
	TickType_t xLastWakeTime;
	const TickType_t xDelay = pdMS_TO_TICKS( 100 );

	xLastWakeTime = xTaskGetTickCount();

	for( ;; ) {
		if (xSemaphoreTake(framebuffer_mutex, portMAX_DELAY) == pdTRUE) {
			add_score();
			fb_flush();
			if (xSemaphoreGive(framebuffer_mutex) == pdFALSE) {}
		}
		vTaskDelayUntil( &xLastWakeTime, xDelay );
	}
}

static void vTaskButton( void * pvParameters )
{
	for( ;; ) {
		if (xSemaphoreTake( sem_interrupt, portMAX_DELAY) == pdPASS) {
			move_paddle(interrupt_color);
		}
	}
}


