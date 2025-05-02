#include "headers.h"

bool drive = true; // true is drive, false is reverse.

void GearCheckTask(void *pvParameters)
{
    /* Initialize periodic timing */
    TickType_t xLastWakeTime;
    const TickType_t xFrequency = pdMS_TO_TICKS(50); // Check every 50ms
    
    xLastWakeTime = xTaskGetTickCount();
    
    /* Task loop */
    while(1)
    {
			
        /* Read the gear state */
        drive = !GET_BIT(GPIO_PORTF_DATA_R, ONE);
        
        
        
        /* Wait until next check time */
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}
