/* FreeRTOS.org includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "startup.h"
#include "wrap.h"
#include "display.h"
#include "framebuffer.h"
#include "pong_field.h"

/*-----------------------------------------------------------*/

static uint8_t direction = LEFT;

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

/*-----------------------------------------------------------*/
int main( void )
{
	/* _init for uart printf */
	_init();
	/* three tasks with different priorities */
	oled_init();
	init_pong();
	delay(2000);
	xTaskCreate( vTaskField, "Field", 1000, (void*)pcTextForTask1, 2, NULL );
	xTaskCreate( vTask2, "Task 2", 1000, (void*)pcTextForTask2, 1, NULL );
	xTaskCreate( vTask3, "Task 3", 1000, (void*)pcTextForTask3, 1, NULL );

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
		printf("print field!\n");
		fb_init();
		oled_clear();
		write_field_to_framebuffer();
		fb_flush();
		if (direction == LEFT) {
			direction = !move_paddle_up(LEFT);
			move_paddle_down(RIGHT);
		} else {
			direction = move_paddle_down(LEFT);
			move_paddle_up(RIGHT);
		}
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
