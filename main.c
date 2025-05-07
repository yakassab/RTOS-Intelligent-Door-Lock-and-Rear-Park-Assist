#include "headers.h"
#include <stdbool.h>
#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>
#include <semphr.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/sysctl.h"

// ======== CONNECTIONS: ========
/*
    PA6 to SCL
    PA7 to SDA
    
    PF1 to LED1 (Gear indicator)
    PF2 to LED2 (Distance indicator)
    PF3 to LED3 (Distance indicator)
    PF4 to Buzzer
    
    PB0 to Trig
    PB3 to Echo
    
    PE2 to Potentiometer
    PE3 to Ignition
    PE1 to Gear
    PE5 to Lock
    
    PD2 to Door
*/

/************************************************************************************************************************************************
                                                            VARIABLES DEFINITION
                                                "Semaphores, Mutexes, Handlers, Variables"
*************************************************************************************************************************************************/

/* Global Variables */
#define LOCKED            1
#define UNLOCKED          0
#define REVERSE_GEAR      1
#define NORMAL_GEAR       0
#define IGNITION_ON       1
#define IGNITION_OFF      0
#define DOOR_OPEN         1
#define DOOR_CLOSED       0

/* System state variables */
bool ignition_state = IGNITION_OFF;
bool gear_state = NORMAL_GEAR;
bool door_state = DOOR_CLOSED;
bool lock_state = UNLOCKED;
float distance = 0;

/* Handlers, Semaphores */
TaskHandle_t UltrasonicHandle = NULL;
SemaphoreHandle_t xIgnitionSemaphore = NULL;
SemaphoreHandle_t xGearSemaphore = NULL;
SemaphoreHandle_t xDoorSemaphore = NULL;
SemaphoreHandle_t xLockSemaphore = NULL;
SemaphoreHandle_t xUltrasonicSemaphore = NULL;
SemaphoreHandle_t xBuzzerSemaphore = NULL;
SemaphoreHandle_t xDataMutex = NULL;

/************************************************************************************************************************************************
                                                           TASKS PROTOTYPE
*************************************************************************************************************************************************/

void IgnitionCheckTask(void *pvParameters);
void GearCheckTask(void *pvParameters);
void DoorCheckTask(void *pvParameters);
void vUltrasonicTask(void *pvParameters);
void vSpeedTask(void *pvParameters);
void vBuzzerTask(void *pvParameters);
void DisplayTask(void *pvParameters);
void ISRHandlers(void);
void InterruptInit(void);

/************************************************************************************************************************************************
                                                        INTERRUPT CONFIGURATION
*************************************************************************************************************************************************/

void InterruptInit(void) {
    // Register ISR handlers for both ports
    GPIOIntRegister(GPIO_PORTE_BASE, ISRHandlers);
    GPIOIntRegister(GPIO_PORTD_BASE, ISRHandlers);
    
    // Set interrupt triggering types for PORT E pins
    GPIOIntTypeSet(GPIO_PORTE_BASE, GPIO_PIN_3, GPIO_BOTH_EDGES); // IGNITION
    GPIOIntTypeSet(GPIO_PORTE_BASE, GPIO_PIN_1, GPIO_BOTH_EDGES); // GEAR
    GPIOIntTypeSet(GPIO_PORTE_BASE, GPIO_PIN_5, GPIO_BOTH_EDGES); // LOCK
    
    // Set interrupt triggering types for PORT D pins
    GPIOIntTypeSet(GPIO_PORTD_BASE, GPIO_PIN_2, GPIO_BOTH_EDGES); // DOOR
    
    // Set interrupt priorities
    IntPrioritySet(PORT_E, configMAX_SYSCALL_INTERRUPT_PRIORITY);
    IntPrioritySet(PORT_D, configMAX_SYSCALL_INTERRUPT_PRIORITY+1);
    
    // Enable interrupts for PORT E pins
    GPIOIntEnable(GPIO_PORTE_BASE, GPIO_PIN_3); // IGNITION
    GPIOIntEnable(GPIO_PORTE_BASE, GPIO_PIN_1); // GEAR
    GPIOIntEnable(GPIO_PORTE_BASE, GPIO_PIN_5); // LOCK
    
    // Enable interrupts for PORT D pins
    GPIOIntEnable(GPIO_PORTD_BASE, GPIO_PIN_2); // DOOR
}

/************************************************************************************************************************************************
                                                        MAIN FUNCTION
*************************************************************************************************************************************************/

int main(void)
{
    DIO_Init(PORT_F);
    DIO_Init(PORT_D);
    DIO_Init(PORT_E);

    I2C1_Init(); // (PA6 to SCL and PA7 to SDA)
    LCD_init();
    LCD_command(LCD_CLEAR);
    
    Potentiometer_Init(); // PE2 to (Potentiometer)
    delay_ms(100);
    Ultrasonic_Init();
    delay_ms(100);

	
		
    // Configure input pins
    DIO_Configure(PORT_E, PIN_THREE, DIGITAL, INPUT, PDR); // IGNITION
    DIO_Configure(PORT_E, PIN_ONE, DIGITAL, INPUT, PDR);   // GEAR
    DIO_Configure(PORT_D, PIN_TWO, DIGITAL, INPUT, PDR);   // DOOR
    DIO_Configure(PORT_E, PIN_FIVE, DIGITAL, INPUT, PDR);  // LOCK

    // Configure output pins
    DIO_Configure(PORT_F, PIN_ONE, DIGITAL, OUTPUT, PDR);   // LED1
    DIO_Configure(PORT_F, PIN_TWO, DIGITAL, OUTPUT, PDR);   // LED2
    DIO_Configure(PORT_F, PIN_THREE, DIGITAL, OUTPUT, PDR); // LED3
    DIO_Configure(PORT_F, PIN_FOUR, DIGITAL, OUTPUT, PDR);  // BUZZER

    // Create semaphores
    xIgnitionSemaphore = xSemaphoreCreateBinary();
    xGearSemaphore = xSemaphoreCreateBinary();
    xDoorSemaphore = xSemaphoreCreateBinary();
    xLockSemaphore = xSemaphoreCreateBinary();
    xUltrasonicSemaphore = xSemaphoreCreateBinary();
    xBuzzerSemaphore = xSemaphoreCreateBinary();
    xDataMutex = xSemaphoreCreateMutex();
    
    // Initialize interrupts
    InterruptInit();
    
    // Create tasks with appropriate priorities
    xTaskCreate(IgnitionCheckTask, "Ignition Task", 128, NULL, 4, NULL); 
    xTaskCreate(vUltrasonicTask, "UltraTask", 128, NULL, 3, &UltrasonicHandle);
    xTaskCreate(GearCheckTask, "Gear Task", 128, NULL, 2, NULL); 
    xTaskCreate(vSpeedTask, "Speed Task", 128, NULL, 2, NULL); 
    xTaskCreate(vBuzzerTask, "BuzzerTask", 128, NULL, 2, NULL);
    xTaskCreate(DoorCheckTask, "DoorTask", 128, NULL, 2, NULL);
    xTaskCreate(DisplayTask, "Display Task", 128, NULL, 1, NULL); 
    
    // Start the scheduler
    vTaskStartScheduler();
    
    // Should never reach here if scheduler started properly
    while(1) {
        // Error handler
    }
}

/************************************************************************************************************************************************
                                                        ISR HANDLER
*************************************************************************************************************************************************/

void ISRHandlers(void) {
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    
    // Check if interrupt came from IGNITION switch (PORT_E, PIN_3)
    if (GPIOIntStatus(GPIO_PORTE_BASE, true) & GPIO_PIN_3) {
        // Clear the interrupt flag
        GPIOIntClear(GPIO_PORTE_BASE, GPIO_PIN_3);
        
        // Give semaphore to wake up ignition task
        xSemaphoreGiveFromISR(xIgnitionSemaphore, &xHigherPriorityTaskWoken);
    }
    
    // Check if interrupt came from GEAR switch (PORT_E, PIN_1)
    else if (GPIOIntStatus(GPIO_PORTE_BASE, true) & GPIO_PIN_1) {
        // Clear the interrupt flag
        GPIOIntClear(GPIO_PORTE_BASE, GPIO_PIN_1);
        
        // Give semaphore to wake up gear task
        xSemaphoreGiveFromISR(xGearSemaphore, &xHigherPriorityTaskWoken);
    }
    
    // Check if interrupt came from DOOR switch (PORT_D, PIN_2)
    else if (GPIOIntStatus(GPIO_PORTD_BASE, true) & GPIO_PIN_2) {
        // Clear the interrupt flag
        GPIOIntClear(GPIO_PORTD_BASE, GPIO_PIN_2);
        
        // Give semaphore to wake up door task
        xSemaphoreGiveFromISR(xDoorSemaphore, &xHigherPriorityTaskWoken);
    }
    
    // Check if interrupt came from LOCK switch (PORT_E, PIN_5)
    else if (GPIOIntStatus(GPIO_PORTE_BASE, true) & GPIO_PIN_5) {
        // Clear the interrupt flag
        GPIOIntClear(GPIO_PORTE_BASE, GPIO_PIN_5);
        
        // Give semaphore to wake up lock task
        xSemaphoreGiveFromISR(xLockSemaphore, &xHigherPriorityTaskWoken);
    }
    
    // If giving the semaphore unblocked a higher-priority task, yield to it
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

/************************************************************************************************************************************************
                                                        TASK IMPLEMENTATIONS
*************************************************************************************************************************************************/

void IgnitionCheckTask(void *pvParameters) {
    // Take semaphore initially to block task until triggered
    xSemaphoreTake(xIgnitionSemaphore, 0);
    
    while(1) {
        // Wait for semaphore (released by interrupt when ignition state changes)
        xSemaphoreTake(xIgnitionSemaphore, portMAX_DELAY);
        
        // Delay for debounce
        vTaskDelay(50 / portTICK_PERIOD_MS);
        
        // Acquire mutex before accessing shared data
        xSemaphoreTake(xDataMutex, portMAX_DELAY);
        
        // Read ignition state
        ignition_state = GET_BIT(GPIO_PORTE_DATA_R, THREE);
        
        if (ignition_state == IGNITION_ON) {
            // Auto-lock doors when ignition is on
            lock_state = LOCKED;
            DIO_WritePin(PORT_F, PIN_ONE, ONE); // Turn on LED indicator
        } else {
            // Unlock doors when ignition is off
            lock_state = UNLOCKED;
            DIO_WritePin(PORT_F, PIN_ONE, ZERO); // Turn off LED indicator
        }
        
        // Release mutex after updating shared data
        xSemaphoreGive(xDataMutex);
    }
}

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

void vUltrasonicTask(void *pvParameters) {
    // Take semaphore initially to block task until triggered by gear task
    xSemaphoreTake(xUltrasonicSemaphore, 0);
    
    while(1) {
        // Wait for semaphore or check periodically when in reverse gear
        if (xSemaphoreTake(xUltrasonicSemaphore, 100 / portTICK_PERIOD_MS) == pdTRUE || 
            (gear_state == REVERSE_GEAR)) {
            
            // Acquire mutex before accessing shared data
            xSemaphoreTake(xDataMutex, portMAX_DELAY);
            
            // Measure distance with ultrasonic sensor
            distance = Ultrasonic_GetDistance();  // Using correct function name
            
            // Release mutex after updating shared data
            xSemaphoreGive(xDataMutex);
            
            // Signal buzzer task to respond to new distance
            xSemaphoreGive(xBuzzerSemaphore);
            
            // Continue measurements while in reverse gear
            if (gear_state == REVERSE_GEAR) {
                vTaskDelay(100 / portTICK_PERIOD_MS);
                xSemaphoreGive(xUltrasonicSemaphore); // Re-trigger self for continuous measurement
            }
        }
    }
}

void vSpeedTask(void *pvParameters) {
    while(1) {
        // Acquire mutex before accessing shared data
        xSemaphoreTake(xDataMutex, portMAX_DELAY);
        
        // Read speed from potentiometer
        speed = Potentiometer_GetSpeed();  // Using correct function name
        
        // Release mutex after updating shared data
        xSemaphoreGive(xDataMutex);
        
        // Check speed periodically
        vTaskDelay(30 / portTICK_PERIOD_MS);
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
                DIO_WritePin(PORT_F, PIN_ONE, ONE);
                DIO_WritePin(PORT_F, PIN_TWO, ONE);
                DIO_WritePin(PORT_F, PIN_THREE, ONE);
                DIO_WritePin(PORT_F, PIN_FOUR, ONE); // Continuous beep
            } else if (local_distance < 30) {
                // Medium distance - fast beep
                DIO_WritePin(PORT_F, PIN_ONE, ONE);
                DIO_WritePin(PORT_F, PIN_TWO, ONE);
                DIO_WritePin(PORT_F, PIN_THREE, ZERO);
                DIO_WritePin(PORT_F, PIN_FOUR, ONE); // Beep
                vTaskDelay(100 / portTICK_PERIOD_MS);
                DIO_WritePin(PORT_F, PIN_FOUR, ZERO);
                vTaskDelay(100 / portTICK_PERIOD_MS);
                xSemaphoreGive(xBuzzerSemaphore); // Re-trigger self
            } else if (local_distance < 50) {
                // Far distance - slow beep
                DIO_WritePin(PORT_F, PIN_ONE, ONE);
                DIO_WritePin(PORT_F, PIN_TWO, ZERO);
                DIO_WritePin(PORT_F, PIN_THREE, ZERO);
                DIO_WritePin(PORT_F, PIN_FOUR, ONE); // Beep
                vTaskDelay(200 / portTICK_PERIOD_MS);
                DIO_WritePin(PORT_F, PIN_FOUR, ZERO);
                vTaskDelay(500 / portTICK_PERIOD_MS);
                xSemaphoreGive(xBuzzerSemaphore); // Re-trigger self
            } else {
                // No obstacle detected
                DIO_WritePin(PORT_F, PIN_ONE, ZERO);
                DIO_WritePin(PORT_F, PIN_TWO, ZERO);
                DIO_WritePin(PORT_F, PIN_THREE, ZERO);
                DIO_WritePin(PORT_F, PIN_FOUR, ZERO);
            }
        }
    }
}

void DisplayTask(void *pvParameters) {
    while(1) {
        // Acquire mutex to safely read shared data
        xSemaphoreTake(xDataMutex, portMAX_DELAY);
        
        // Make local copies of shared data
        bool local_ignition = ignition_state;
        bool local_gear = gear_state;
        bool local_door = door_state;
        bool local_lock = lock_state;
        int local_speed = speed;
        float local_distance = distance;
        
        // Release mutex as soon as possible
        xSemaphoreGive(xDataMutex);
        
        // Clear display
        LCD_command(LCD_CLEAR);
        
        // Update first line of display
        LCD_set_cursor(0, 0);
        if (local_ignition == IGNITION_ON) {
            LCD_write_string("IGN:ON ");
        } else {
            LCD_write_string("IGN:OFF ");
        }
        
        if (local_gear == REVERSE_GEAR) {
            LCD_write_string("GEAR:R");
        } else {
            LCD_write_string("GEAR:D");
        }
        
        // Update second line of display
        LCD_set_cursor(1, 0);
        if (local_gear == REVERSE_GEAR) {
            LCD_write_string("DIST:");
            LCD_print_int(local_distance);
            LCD_write_string("cm");
        } else {
            LCD_write_string("SPEED:");
            LCD_print_int(local_speed);
            if (local_door == DOOR_OPEN) {
                LCD_write_string(" DOOR!");
            }
        }
        
        // Update display periodically
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}

/* Idle Task for sleeping the processor */
void vApplicationIdleHook(void) {
    /* Put the microcontroller in a low power mode */
    //SysCtlSleep();
}