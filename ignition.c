#include "headers.h"
#include "ignition.h"
#include <stdbool.h>

// Define variables
bool ignition = false;
bool ignition_changed = false;
bool ignition_state = IGNITION_OFF;

SemaphoreHandle_t xIgnitionSemaphore = NULL;


void check_ignition(void) {
    bool new_state;
    new_state = GET_BIT(GPIO_PORTE_DATA_R, THREE);
    if (new_state != ignition) {
        ignition = new_state;
        ignition_changed = true;
    }
}

void IgnitionCheckTask(void *pvParameters) {
    // Take semaphore initially to block task until triggered
    xSemaphoreTake(xIgnitionSemaphore, 0);
    bool new_state;
    while(1) {
        // Wait for semaphore (released by interrupt when ignition state changes)
        xSemaphoreTake(xIgnitionSemaphore, portMAX_DELAY);
        
        // Delay for debounce
        vTaskDelay(50 / portTICK_PERIOD_MS);
        
        // Acquire mutex before accessing shared data
        xSemaphoreTake(xDataMutex, portMAX_DELAY);
			
			new_state = GET_BIT(GPIO_PORTE_DATA_R, THREE);
			if (new_state != ignition_state) {
        ignition_state = new_state;
        ignition_changed = true;
			} 
       
        
        // Release mutex after updating shared data
        xSemaphoreGive(xDataMutex);
    }
}
//void IgnitionCheckTask(void *pvParameters)
//{
//    /* Initialize periodic timing */
//    TickType_t xLastWakeTime;
//    const TickType_t xFrequency = pdMS_TO_TICKS(50); // Check every 50ms
//    
//    xLastWakeTime = xTaskGetTickCount();
//    
//    /* Task loop */
//    while(1)
//    {
//			
//				bool new_state;
//        /* Read the ignition state */
//        new_state = GET_BIT(GPIO_PORTE_DATA_R, THREE);
//        
//        /* Check if ignition state has changed */
//        if (new_state != ignition)
//        {
//            ignition = new_state;
//            ignition_changed = true;
//					if (!ignition)door_locked = false;
//        }
//        
//        /* Wait until next check time */
//        vTaskDelayUntil(&xLastWakeTime, xFrequency);
//    }
//}



