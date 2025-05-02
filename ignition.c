#include "headers.h"

bool ignition = false;
bool ignition_changed = false;

void IgnitionCheckTask(void *pvParameters)
{
    /* Initialize periodic timing */
    TickType_t xLastWakeTime;
    const TickType_t xFrequency = pdMS_TO_TICKS(50); // Check every 50ms
    
    xLastWakeTime = xTaskGetTickCount();
    
    /* Task loop */
    while(1)
    {
			
				bool new_state;
        /* Read the ignition state */
        new_state = GET_BIT(GPIO_PORTE_DATA_R, THREE);
        
        /* Check if ignition state has changed */
        if (new_state != ignition)
        {
            ignition = new_state;
            ignition_changed = true;
        }
        
        /* Wait until next check time */
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}

void check_ignition(void){
	
	bool new_state;
	new_state = GET_BIT(GPIO_PORTE_DATA_R, THREE);
	if (new_state != ignition){
		ignition = new_state;
		ignition_changed = true;
	}
	
	return;
	
}