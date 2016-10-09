/**
 * \file
 *
 * \brief Generic RTOS configuration
 *
 * Copyright (c) 2015 Atmel Corporation. All rights reserved.
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


#ifndef RTOS_H_
#define RTOS_H_

#include "iot_config.h"

#ifdef __FREERTOS__

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#define IoT_xTaskCreate						xTaskCreate
#define IoT_vTaskDelete						vTaskDelete
#define IoT_vTaskStartScheduler				vTaskStartScheduler
#define IoT_vTaskSuspend					vTaskSuspend
#define IoT_vTaskResume						vTaskResume
#define IoT_vTaskSuspendAll					vTaskSuspendAll
#define IoT_xTaskResumeAll					xTaskResumeAll
#define IoT_xTaskResumeFromISR				xTaskResumeFromISR
#define IoT_uxTaskPriorityGet				uxTaskPriorityGet
#define IoT_vTaskPrioritySet				vTaskPrioritySet
#define IoT_xTaskGetCurrentTaskHandle		xTaskGetCurrentTaskHandle		
#define IoT_xTaskGetTickCount				xTaskGetTickCount
#define IoT_uxTaskGetNumberOfTasks			uxTaskGetNumberOfTasks
#define IoT_vTaskList						vTaskList
#define IoT_xQueueCreate					xQueueCreate
#define IoT_vQueueDelete					vQueueDelete
#define IoT_xQueueSend						xQueueSend
#define IoT_xQueueSendToBack				xQueueSendToBack
#define IoT_xQueueSendToFront				xQueueSendToFront
#define IoT_xQueueReceive					xQueueReceive
#define IoT_xQueuePeek						xQueuePeek
#define IoT_uxQueueMessagesWaiting			uxQueueMessagesWaiting
#define IoT_xQueueSendFromISR				xQueueSendFromISR
#define IoT_xQueueSendToBackFromISR			xQueueSendToBackFromISR
#define IoT_xQueueSendToFrontFromISR		xQueueSendToFrontFromISR
#define IoT_xQueueReceiveFromISR			xQueueReceiveFromISR
#define IoT_vSemaphoreCreateBinary			vSemaphoreCreateBinary
#define IoT_xSemaphoreCreateCounting		xSemaphoreCreateCounting
#define IoT_xSemaphoreCreateMutex			xSemaphoreCreateMutex
#define IoT_xSemaphoreTake					xSemaphoreTake
#define IoT_xSemaphoreGive					xSemaphoreGive
#define IoT_xSemaphoreGiveFromISR			xSemaphoreGiveFromISR
#define IoT_vTaskDelay						vTaskDelay
#define IoT_vTaskDelayUntil					vTaskDelayUntil
#define IoT_pvPortMalloc					pvPortMalloc
#define IoT_vPortFree						vPortFree
#define IoT_vPortEnterCritical				vPortEnterCritical
#define IoT_vPortExitCritical				vPortExitCritical
#define IoT_xTimerCreate					xTimerCreate
#define IoT_xTimerStart						xTimerStart
#define IoT_xTimerStartFromISR				xTimerStartFromISR
#define IoT_vPortYieldFromISR				vPortYieldFromISR
#else

#define IoT_xTaskCreate						
#define IoT_vTaskDelete						
#define IoT_vTaskStartScheduler				
#define IoT_vTaskSuspend					
#define IoT_vTaskResume						
#define IoT_vTaskSuspendAll					
#define IoT_vTaskSuspendAll					
#define IoT_xTaskResumeFromISR				
#define IoT_uxTaskPriorityGet				
#define IoT_vTaskPrioritySet				
#define IoT_xTaskGetCurrentTaskHandle		
#define IoT_xTaskGetTickCount				
#define IoT_uxTaskGetNumberOfTasks			
#define IoT_vTaskList						
#define IoT_xQueueCreate					
#define IoT_vQueueDelete					
#define IoT_xQueueSend						
#define IoT_xQueueSendToBack				
#define IoT_xQueueSendToFront				
#define IoT_xQueueReceive					
#define IoT_xQueuePeek						
#define IoT_uxQueueMessagesWaiting			
#define IoT_xQueueSendFromISR				
#define IoT_xQueueSendToBackFromISR			
#define IoT_xQueueSendToFrontFromISR		
#define IoT_xQueueReceiveFromISR			
#define IoT_vSemaphoreCreateBinary			
#define IoT_xSemaphoreCreateCounting		
#define IoT_xSemaphoreCreateMutex			
#define IoT_xSemaphoreTake					
#define IoT_xSemaphoreGive					
#define IoT_xSemaphoreGiveFromISR			
#define IoT_vTaskDelay						
#define IoT_vTaskDelayUntil					
#define IoT_pvPortMalloc							
#define IoT_vPortFree
#define IoT_vPortEnterCritical
#define IoT_vPortExitCritical
#define IoT_xTimerCreate
#define IoT_xTimerStart	
#define IoT_xTimerStartFromISR						
#define IoT_vPortYieldFromISR

#endif

#define IoT_configASSERT( x ) if( ( x ) == 0 ) { IoT_vPortEnterCritical(); for( ;; ) __asm volatile( "NOP" ); }

#endif /* RTOS_H_ */