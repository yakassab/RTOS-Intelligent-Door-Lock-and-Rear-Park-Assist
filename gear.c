#include "headers.h"

bool drive = true; // true is drive, false is reverse.

bool gear_state = NORMAL_GEAR;

SemaphoreHandle_t xGearSemaphore = NULL;



void GearCheckTask(void *pvParameters) {
    // Take semaphore initially to block task until triggered
    xSemaphoreTake(xGearSemaphore, 0);
    
    while(1) {
        // Wait for semaphore (released by interrupt when gear state changes)
        xSemaphoreTake(xGearSemaphore, portMAX_DELAY);
        
        // Delay for debounce
        vTaskDelay(50 / portTICK_PERIOD_MS);
        
        // Acquire mutex before accessing shared data
        xSemaphoreTake(xDataMutex, portMAX_DELAY);
        
        // Read gear state
        gear_state = GET_BIT(GPIO_PORTE_DATA_R, ONE);
        
        if (gear_state == REVERSE_GEAR) {
            // Activate the park assist system
            xSemaphoreGive(xUltrasonicSemaphore);
        } else {
            // Turn off all indicators when not in reverse
            DIO_WritePin(PORT_F, PIN_TWO, ZERO);
            DIO_WritePin(PORT_F, PIN_THREE, ZERO);
            DIO_WritePin(PORT_F, PIN_FOUR, ZERO); // Turn off buzzer
        }
        
        // Release mutex after updating shared data
        xSemaphoreGive(xDataMutex);
    }
}


//void GearCheckTask(void *pvParameters)
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
//      /* Read the gear state */
//      drive = !GET_BIT(GPIO_PORTE_DATA_R, ONE);
//			

//        
//        /* Wait until next check time */
//        vTaskDelayUntil(&xLastWakeTime, xFrequency);
//    }
//}
