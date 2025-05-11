#include "headers.h"

bool door_opened = false;
bool door_locked = false;
bool lock_changed = false;
bool door_changed = false;

bool cant_unlock_door = false;

bool first = true;

bool lock_state = UNLOCKED;
bool door_state = DOOR_CLOSED;


SemaphoreHandle_t xDoorSemaphore = NULL;
SemaphoreHandle_t xLockSemaphore = NULL;



bool manual_override = false;


void DoorCheckTask(void *pvParameters) {
    // Take semaphore initially to block task until triggered
    xSemaphoreTake(xDoorSemaphore, 0);
	
		bool prev_door_state = door_state;
    
    while(1) {
        // Wait for semaphore (released by interrupt when door state changes)
        xSemaphoreTake(xDoorSemaphore, portMAX_DELAY);
        
        // Delay for debounce
        vTaskDelay(50 / portTICK_PERIOD_MS);
        
        // Acquire mutex before accessing shared data
        xSemaphoreTake(xDataMutex, portMAX_DELAY);
			
				prev_door_state = door_state;
        
        // Read door state
				if (!door_locked){
        door_state = GET_BIT(GPIO_PORTD_DATA_R, TWO);
        } else if(door_locked && GET_BIT(GPIO_PORTD_DATA_R, TWO)){
					cant_unlock_door = true;
				}
				
				// Check if door state has changed
        if (prev_door_state != door_state) {
            door_changed = true;
        }
				
        // Security feature: if door is opened while ignition is on and car is moving
        if (door_state == DOOR_OPEN && ignition_state == IGNITION_ON && speed > 3) {
            // Sound alarm
            DIO_WritePin(PORT_F, PIN_FOUR, ONE); // Activate buzzer
            vTaskDelay(500 / portTICK_PERIOD_MS);
            DIO_WritePin(PORT_F, PIN_FOUR, ZERO); // Deactivate buzzer
        }
        
        // Release mutex after accessing shared data
        xSemaphoreGive(xDataMutex);
    }
}

// Task to monitor speed and control door locks
void DoorLockTask(void *pvParameters) {
    TickType_t xLastWakeTime;
    const TickType_t xFrequency = pdMS_TO_TICKS(100); // Check every 100ms
    uint8_t prevSwitchState = 0;
    uint8_t currentSwitchState = 0;
    bool prev_lock_state = lock_state; // Track previous lock state
    
    xLastWakeTime = xTaskGetTickCount();
    
    while(1) {
        // Acquire mutex before accessing shared data
        xSemaphoreTake(xDataMutex, portMAX_DELAY);
        
        // Check if speed exceeds 20km/h
        if (ignition_state == IGNITION_ON && speed > 10) {
            speed_lock = true;	
        }
        
        // Read DIP switch state for manual override
        currentSwitchState = GET_BIT(GPIO_PORTE_DATA_R, FIVE);
        
        // Detect switch state change for manual override
        if (prevSwitchState != currentSwitchState) {
            manual_override = true;
            prevSwitchState = currentSwitchState;
        }
        
        // Store previous lock state for comparison
        prev_lock_state = lock_state;
        
        // Update door lock state
        // If manual override is active, use switch state
        // Otherwise use speed_lock
        if (first && speed_lock) {
            lock_state = speed_lock ? LOCKED : UNLOCKED;
					  manual_override = false;
						first = false;
        } else if (manual_override)   {
					            lock_state = currentSwitchState ? LOCKED : UNLOCKED;

        }
				
				
        
        // Check if lock state has changed
        if (prev_lock_state != lock_state) {
            lock_changed = true;
        }
        
        // Apply lock state to actual door lock mechanism
        if (lock_state == LOCKED) {
            door_locked = true;
        } else {
            door_locked = false;
        }
				
				
        
        // Release mutex after accessing shared data
        xSemaphoreGive(xDataMutex);
        
        // Wait until next check time
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
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