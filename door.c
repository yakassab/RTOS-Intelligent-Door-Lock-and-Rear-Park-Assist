#include "headers.h"

bool door_opened = false;
bool door_locked = false;

bool first = true;

void DoorCheckTask(void *pvParameters)
{
    /* Initialize periodic timing */
    TickType_t xLastWakeTime;
    const TickType_t xFrequency = pdMS_TO_TICKS(50); // Check every 50ms
    
    xLastWakeTime = xTaskGetTickCount();
    
    /* Task loop */
    while(1)
    {
			
				door_opened = GET_BIT(GPIO_PORTD_DATA_R, TWO);
				door_locked = GET_BIT(GPIO_PORTE_DATA_R, FIVE);

        
        
        
        /* Wait until next check time */
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}