
// System files
#include <stdio.h>
#include <stdlib.h>

// FreeRTOS files
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"

// Project files
#include "stm32l053xx.h"

static unsigned long long prvSystick = 0;

void vAssertCalled( unsigned long ulLine, const char * const pcFileName )
{
    for( ;; );
}

void vApplicationMallocFailedHook( void )
{
    /* vApplicationMallocFailedHook() will only be called if
    configUSE_MALLOC_FAILED_HOOK is set to 1 in FreeRTOSConfig.h.  It is a hook
    function that will get called if a call to pvPortMalloc() fails.
    pvPortMalloc() is called internally by the kernel whenever a task, queue,
    timer or semaphore is created.  It is also called by various parts of the
    demo application.  If heap_1.c or heap_2.c are used, then the size of the
    heap available to pvPortMalloc() is defined by configTOTAL_HEAP_SIZE in
    FreeRTOSConfig.h, and the xPortGetFreeHeapSize() API function can be used
    to query the size of free heap space that remains (although it does not
    provide information on how the remaining heap might be fragmented). */
    taskDISABLE_INTERRUPTS();
    for( ;; );
}

void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName )
{
    ( void ) pcTaskName;
    ( void ) pxTask;

    /* Run time stack overflow checking is performed if
    configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
    function is called if a stack overflow is detected. */
    taskDISABLE_INTERRUPTS();
    for( ;; );
}

void vApplicationTickHook( void )
{
    /* This function will be called by each tick interrupt if
    configUSE_TICK_HOOK is set to 1 in FreeRTOSConfig.h.  User code can be
    added here, but the tick hook is called from an interrupt context, so
    code must not attempt to block, and only the interrupt safe FreeRTOS API
    functions can be used (those that end in FromISR()).  The code in this
    tick hook implementation is for demonstration only - it has no real
    purpose.  It just gives a semaphore every 50ms.  The semaphore unblocks a
    task that then toggles an LED.  Additionally, the call to
    vQueueSetAccessQueueSetFromISR() is part of the "standard demo tasks"
    functionality. */
    prvSystick++;
}

void vApplicationIdleHook( void )
{
    /* vApplicationIdleHook() will only be called if configUSE_IDLE_HOOK is set
    to 1 in FreeRTOSConfig.h.  It will be called on each iteration of the idle
    task.  It is essential that code added to this hook function never attempts
    to block in any way (for example, call xQueueReceive() with a block time
    specified, or call vTaskDelay()).  If the application makes use of the
    vTaskDelete() API function (as this demo application does) then it is also
    important that vApplicationIdleHook() is permitted to return to its calling
    function, because it is the responsibility of the idle task to clean up
    memory allocated by the kernel to any task that has since been deleted. */
}

static xQueueHandle prvQueue = 0;
static int prvProducerCounter = 0;
static int prvConsumerCounter = 0;

static void prvProducer( void *pvParameters )
{
    for( ;; )
    {
        prvProducerCounter++;
        xQueueSendToBack( prvQueue, &prvProducerCounter, ( TickType_t )0 );
    }
}

static void prvConsumer( void *pvParameters )
{
    for( ;; )
    {
        xQueueReceive( prvQueue, &prvConsumerCounter, ( TickType_t )0 );
    }
}

/**
 * @brief Program main
 *
 * @param None
 *
 * @retval None
 */
int main( void )
{
    BaseType_t xStatus;
    
    // Setup STM32 system (clock, PLL and Flash configuration).
    SystemInit();
    
    // Start producer thread.
    xStatus = xTaskCreate( prvProducer, "Producer", configMINIMAL_STACK_SIZE, NULL, 1, NULL );
    configASSERT( pdPASS == xStatus );

    // Start consumer thread.
    xStatus = xTaskCreate( prvConsumer, "Consumer", configMINIMAL_STACK_SIZE, NULL, 1, NULL );
    configASSERT( pdPASS == xStatus );
    
    // Create communication queue.
    prvQueue = xQueueCreate( 100, sizeof( int ) );
    configASSERT( 0 != prvQueue );
    
    // Start the tasks and timer running.
    vTaskStartScheduler();
    
    // Main loop.
    while ( 1 );
}
