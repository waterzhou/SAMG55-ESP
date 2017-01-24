/**
 * \file
 *
 * \brief Main functions for USB host mass storage example
 *
 * Copyright (C) 2012-2015 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include "conf_usb_host.h"
#include <asf.h>
#include "config/rtos.h"
#include "temperature.h"
#include "camera.h"
#include "wifiserial.h"


#define STRING_EOL    "\r"
#define STRING_HEADER "--IoT Daakin V1 --\r\n" \
"-- "BOARD_NAME" --\r\n" \
"-- Compiled: "__DATE__" "__TIME__" --"STRING_EOL


#define STACK_SIZE_WIFI_TASK		(1024)
#define STACK_SIZE_CAMERA_TASK		(1024)
#define STACK_SIZE_TSENSOR_TASK     (1024)


extern void vApplicationStackOverflowHook(xTaskHandle *pxTask,signed char *pcTaskName);
extern void vApplicationIdleHook(void);
extern void vApplicationTickHook(void);
extern void vApplicationMallocFailedHook(void);
extern void xPortSysTickHandler(void);


/*
 * Sets up the hardware ready to run this example.
 */
static void prvSetupHardware(void);
/**
 * \brief Handler for System Tick interrupt.
 */
void SysTick_Handler(void)
{
	xPortSysTickHandler();
}
/**
 * \brief Called if stack overflow during execution
 */
extern void vApplicationStackOverflowHook(xTaskHandle *pxTask,signed char *pcTaskName)
{
	printf("stack overflow\r\n");
	for (;;) {
	}
}
extern void vApplicationIdleHook(void)
{
}
/**
 * \brief This function is called by FreeRTOS each tick
 */
extern void vApplicationTickHook(void)
{

}
extern void vApplicationMallocFailedHook(void)
{
	/* Called if a call to pvPortMalloc() fails because there is insufficient
	free memory available in the FreeRTOS heap.  pvPortMalloc() is called
	internally by FreeRTOS API functions that create tasks, queues, software
	timers, and semaphores.  The size of the FreeRTOS heap is set by the
	configTOTAL_HEAP_SIZE configuration constant in FreeRTOSConfig.h. */
	/* Force an assert. */
	configASSERT( ( volatile void * ) NULL );
}


/**
 *  \brief Configure UART console.
 */
static void configure_console(void)
{
	const usart_serial_options_t uart_serial_options = {
		.baudrate = CONF_UART_BAUDRATE,
		.charlength = CONF_UART_CHAR_LENGTH,
		.paritytype = CONF_UART_PARITY,
		.stopbits = CONF_UART_STOP_BITS,
	};

	/* Configure console UART. */
	sysclk_enable_peripheral_clock(CONSOLE_UART_ID);
	stdio_serial_init(CONF_UART_CONSOLE, &uart_serial_options);
	
}
static void prvSetupHardware(void)
{
	/* Initialize the synchronous clock system to the default configuration
	   set in conf_clock.h.
	   \note All non-essential peripheral clocks are initially disabled. */
	sysclk_init();
	
	/* Ensure all priority bits are assigned as preemption priority bits. */
	NVIC_SetPriorityGrouping(0);
		
	/* Initialize the resources used by this example to the default
	   configuration set in conf_board.h */
	board_init();
	
	configure_console();
	
	puts(STRING_HEADER);
	
	//tSensor_serial_init();
	
	/* Initialize interrupts */
	irq_initialize_vectors();
	cpu_irq_enable();

	/* Initialize the sleep manager, lock initial mode. */
	sleepmgr_init();

	/* Initialize the user interface */
	//ui_init();

	/* Start USB host stack */
	//uhc_start();
	wifi_serial_init(BIT_RATE_3686400);
	
}
/*! \brief Main function. Execution starts here.
 */
xTaskHandle xHandleCamera = NULL;
xTaskHandle xHandleTemperature = NULL;
int main(void)
{
	/* Prepare the hardware to run this demo. */
	prvSetupHardware();

	
	//task for processing Data from WIFI
	if (xTaskCreate(wifi_task, "Wifi", STACK_SIZE_WIFI_TASK, NULL, tskIDLE_PRIORITY + 4, NULL)!=pdPASS){
		printf("Failed to create taskWifi task\r\n");
	}

	//task for processing Camera Data
	if (xTaskCreate(taskCamera, "Camera", STACK_SIZE_CAMERA_TASK, NULL, tskIDLE_PRIORITY + 3, &xHandleCamera)!=pdPASS){
		printf("Failed to create Monitor task\r\n");
	}
	//task for processing temperature module data
	if (xTaskCreate(sensor_task, "TSensor", STACK_SIZE_TSENSOR_TASK, NULL, tskIDLE_PRIORITY + 3, &xHandleTemperature)!=pdPASS){
		printf("Failed to create Temperature Sensor task\r\n");
	}
	//vTaskSuspend(xHandleTemperature);	
	/* Start the RTOS scheduler. */
	vTaskStartScheduler();
    
	/* The main loop manages only the power mode
	   because the USB management is done by interrupt */
	while (true) {
		//sleepmgr_enter_sleep();
	}
}

/**
 * \mainpage ASF USB host CDC
 *
 * \section intro Introduction
 * This example shows how to implement a USB host CDC
 * on Atmel MCU with USB module.
 *
 * \section startup Startup
 * After loading firmware, connect the board (EVKxx,Xplain,...) to a USB
 * device CDC.
 * The example is a bridge between a USART from the main MCU
 * and the USB host CDC interface.
 *
 * \copydoc UI
 *
 * \section example About example
 *
 * The example uses the following module groups:
 * - Basic modules:
 *   Startup, board, clock, interrupt, power management
 * - USB host stack and CDC modules:
 *   <br>services/usb/
 *   <br>services/usb/uhc/
 *   <br>services/usb/class/cdc/host/
 * - Specific implementation:
 *    - main.c,
 *      <br>initializes clock
 *      <br>initializes interrupt
 *      <br>manages UI
 *    - specific implementation for each target "./examples/product_board/":
 *       - conf_foo.h   configuration of each module
 *       - ui.c        implement of user's interface (buttons, leds)
 */

void assert_triggered(const char *file, uint32_t line)
{
	volatile uint32_t block_var = 0, line_in;
	const char *file_in;

	/* These assignments are made to prevent the compiler optimizing the
	values away. */
	file_in = file;
	line_in = line;
	(void) file_in;
	(void) line_in;

	taskENTER_CRITICAL();
	{
		while (block_var == 0) {
			/* Set block_var to a non-zero value in the debugger to
			step out of this function. */
		}
	}
	taskEXIT_CRITICAL();
}