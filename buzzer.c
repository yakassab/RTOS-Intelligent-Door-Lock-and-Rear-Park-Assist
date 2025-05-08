#include "headers.h"

bool buzz = false;

SemaphoreHandle_t xBuzzerSemaphore = NULL;


void check_buzz(void){
	
	if (!drive){
		DIO_WritePin(PORT_F, PIN_FOUR, 1);
		delay_ms(latest_distance);
				DIO_WritePin(PORT_F, PIN_FOUR, 0);
		delay_ms(latest_distance);

	}

}



void vBuzzerTask(void *pvParameters) {
    // Take semaphore initially to block task until triggered
    xSemaphoreTake(xBuzzerSemaphore, 0);
    
    while(1) {
        // Wait for semaphore (released when distance changes)
        xSemaphoreTake(xBuzzerSemaphore, portMAX_DELAY);
        
        // Acquire mutex to safely read shared data
        xSemaphoreTake(xDataMutex, portMAX_DELAY);
        
        bool local_gear = gear_state;
        float local_distance = distance;
        
        // Release mutex as soon as possible
        xSemaphoreGive(xDataMutex);
        
        // Check if in reverse gear
        if (local_gear == REVERSE_GEAR) {
            // Control buzzer and LEDs based on distance
            if (local_distance < 10) {
                // Very close - continuous beep
								RGB_LED_Red();
                DIO_WritePin(PORT_F, PIN_FOUR, ONE); // Continuous beep
            } else if (local_distance < 30) {
                // Medium distance - fast beep
								RGB_LED_Yellow();

                DIO_WritePin(PORT_F, PIN_FOUR, ONE); // Beep
                vTaskDelay(100 / portTICK_PERIOD_MS);
                DIO_WritePin(PORT_F, PIN_FOUR, ZERO);
                vTaskDelay(100 / portTICK_PERIOD_MS);
                xSemaphoreGive(xBuzzerSemaphore); // Re-trigger self
            } else if (local_distance < 50) {
                // Far distance - slow beep
								RGB_LED_Green();

                DIO_WritePin(PORT_F, PIN_FOUR, ONE); // Beep
                vTaskDelay(200 / portTICK_PERIOD_MS);
                DIO_WritePin(PORT_F, PIN_FOUR, ZERO);
                vTaskDelay(500 / portTICK_PERIOD_MS);
                xSemaphoreGive(xBuzzerSemaphore); // Re-trigger self
            } else {
                // No obstacle detected
								RGB_LED_Green();
                DIO_WritePin(PORT_F, PIN_FOUR, ZERO);
            }
        }
    }
}

//void vBuzzerTask(void *pvParameters)
//{
//    // Task initialization
//    TickType_t xLastWakeTime;
//    xLastWakeTime = xTaskGetTickCount();
//	
//    uint32_t beepDelay;
//	
//    for (;;)
//    {
//        // Check if in reverse mode
//        if (!drive)
//        {

//            
//            
//            // Set minimum and maximum delays to prevent very fast or very slow beeping
//            if (latest_distance < 10) {       
//							RGB_LED_Red();
//							DIO_WritePin(PORT_F, PIN_FOUR, 1);
//							vTaskDelay(pdMS_TO_TICKS(100));
//						} 
//						else if (latest_distance < 30) {beepDelay = 60;RGB_LED_Yellow();}     
//            else if (latest_distance < 50) {beepDelay = 120;RGB_LED_Green();}     
//						else {beepDelay = 200;}     
//						
//            DIO_WritePin(PORT_F, PIN_FOUR, 1);
//            vTaskDelay(pdMS_TO_TICKS(beepDelay));
//            
//            DIO_WritePin(PORT_F, PIN_FOUR, 0);
//            vTaskDelay(pdMS_TO_TICKS(beepDelay));
//        }
//        else
//        {
//            // When in drive mode, just wait a bit before checking again
//						RGB_LED_Off();
//						if (door_opened && speed > 5){
//							DIO_WritePin(PORT_F, PIN_FOUR, 1);

//						} else {
//						DIO_WritePin(PORT_F, PIN_FOUR, 0);
//						}
//            
//        }
//				vTaskDelay(pdMS_TO_TICKS(100));
//    }
//    
//}


// Turn all LEDs off
void RGB_LED_Off(void) {
    GPIO_PORTF_DATA_R &= ~(LED_RED | LED_BLUE | LED_GREEN);
}

// Turn on specific colors (you can combine them for different colors)
void RGB_LED_Set(uint8_t red, uint8_t green, uint8_t blue) {
    uint8_t value = 0;
    
    // Build the value (1 = ON, 0 = OFF)
    if (red)   value |= LED_RED;
    if (green) value |= LED_GREEN;
    if (blue)  value |= LED_BLUE;
    
    // Clear the LED bits first, then set the new values
    GPIO_PORTF_DATA_R = (GPIO_PORTF_DATA_R & ~(LED_RED | LED_GREEN | LED_BLUE)) | value;
}

// Predefined color functions
void RGB_LED_Red(void) {
    RGB_LED_Set(1, 0, 0);  // Red only
}

void RGB_LED_Green(void) {
    RGB_LED_Set(0, 1, 0);  // Green only
}

void RGB_LED_Yellow(void) {
    RGB_LED_Set(1, 1, 0);  // Red + Green = Yellow
}