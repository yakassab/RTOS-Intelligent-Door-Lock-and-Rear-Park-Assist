#include "headers.h"

bool door_opened = false;
bool door_locked = false;

bool lock_state = UNLOCKED;
bool door_state = DOOR_CLOSED;


SemaphoreHandle_t xDoorSemaphore = NULL;
SemaphoreHandle_t xLockSemaphore = NULL;



bool manual_override = false;


void DoorCheckTask(void *pvParameters) {
    // Take semaphore initially to block task until triggered
    xSemaphoreTake(xDoorSemaphore, 0);
    
    while(1) {
        // Wait for semaphore (released by interrupt when door state changes)
        xSemaphoreTake(xDoorSemaphore, portMAX_DELAY);
        
        // Delay for debounce
        vTaskDelay(50 / portTICK_PERIOD_MS);
        
        // Acquire mutex before accessing shared data
        xSemaphoreTake(xDataMutex, portMAX_DELAY);
        
        // Read door state
        door_state = GET_BIT(GPIO_PORTD_DATA_R, TWO);
        
        // Security feature: if door is opened while ignition is on and car is moving
        if (door_state == DOOR_OPEN && ignition_state == IGNITION_ON && speed > 0) {
            // Sound alarm
            DIO_WritePin(PORT_F, PIN_FOUR, ONE); // Activate buzzer
            vTaskDelay(500 / portTICK_PERIOD_MS);
            DIO_WritePin(PORT_F, PIN_FOUR, ZERO); // Deactivate buzzer
        }
        
        // Release mutex after accessing shared data
        xSemaphoreGive(xDataMutex);
    }
}

//void DoorCheckTask(void *pvParameters)
//{
//    /* Initialize periodic timing */
//    TickType_t xLastWakeTime;
//    const TickType_t xFrequency = pdMS_TO_TICKS(50); // Check every 50ms
//    
//    xLastWakeTime = xTaskGetTickCount();
//    uint8 switchState = false;
//		uint8 prevState;
//    /* Task loop */
//    while(1)
//    {
//			
//				door_opened = GET_BIT(GPIO_PORTD_DATA_R, TWO);
//				if (door_locked){
//					door_opened = false;
//				}
//				prevState = switchState;
//				switchState = GET_BIT(GPIO_PORTE_DATA_R, FIVE);

//				if (prevState != switchState){manual_override = true;}
//			
//				door_locked = speed_lock ^ switchState;
//			
//        /* Wait until next check time */
//        vTaskDelayUntil(&xLastWakeTime, xFrequency);
//    }
//}