#include "headers.h"

bool buzz = false;


void check_buzz(void){
	
	if (!drive){
		DIO_WritePin(PORT_F, PIN_FOUR, 1);
		delay_ms(latest_distance);
				DIO_WritePin(PORT_F, PIN_FOUR, 0);
		delay_ms(latest_distance);

	}

}

void vBuzzerTask(void *pvParameters)
{
    // Task initialization
    TickType_t xLastWakeTime;
    xLastWakeTime = xTaskGetTickCount();
    
    for (;;)
    {
        // Check if not in drive mode
        if (!drive)
        {
            // Calculate delay time based on distance, with safety bounds
            uint32_t beepDelay = latest_distance;
            
            // Set minimum and maximum delays to prevent very fast or very slow beeping
            if (beepDelay < 10) beepDelay = 10;     // Minimum 10ms delay
            if (beepDelay == 999) continue;
            
            // Beep pattern: ON for distance ms, then OFF for distance ms
            DIO_WritePin(PORT_F, PIN_FOUR, 1);
            vTaskDelay(pdMS_TO_TICKS(beepDelay));
            
            DIO_WritePin(PORT_F, PIN_FOUR, 0);
            vTaskDelay(pdMS_TO_TICKS(beepDelay));
        }
        else
        {
            // When in drive mode, just wait a bit before checking again
            vTaskDelay(pdMS_TO_TICKS(100));
        }
    }
    
}