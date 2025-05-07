#include "headers.h"

bool door_opened = false;
bool door_locked = false;

bool manual_override = false;

void DoorCheckTask(void *pvParameters)
{
    /* Initialize periodic timing */
    TickType_t xLastWakeTime;
    const TickType_t xFrequency = pdMS_TO_TICKS(50); // Check every 50ms
    
    xLastWakeTime = xTaskGetTickCount();
    uint8 switchState = false;
		uint8 prevState;
    /* Task loop */
    while(1)
    {
			
				door_opened = GET_BIT(GPIO_PORTD_DATA_R, TWO);
				if (door_locked){
					door_opened = false;
				}
				prevState = switchState;
				switchState = GET_BIT(GPIO_PORTE_DATA_R, FIVE);

				if (prevState != switchState){manual_override = true;}
			
				door_locked = speed_lock ^ switchState;
			
        /* Wait until next check time */
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}