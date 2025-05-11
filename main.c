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
    
    PF4 to Buzzer
    
    PB0 to Trig
    PB3 to Echo
		
		PE1 to Gear
    PE2 to Potentiometer
    PE3 to Ignition
    PE5 to Lock
    
    PD2 to Door
*/

SemaphoreHandle_t xDataMutex = NULL;

/************************************************************************************************************************************************
                                                           TASKS PROTOTYPE
*************************************************************************************************************************************************/


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
    DIO_Configure(PORT_F, PIN_ONE, DIGITAL, OUTPUT, PDR);   // LED
    DIO_Configure(PORT_F, PIN_TWO, DIGITAL, OUTPUT, PDR);   // LED
    DIO_Configure(PORT_F, PIN_THREE, DIGITAL, OUTPUT, PDR); // LED
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
		xTaskCreate(DoorLockTask, "DoorLock", 128, NULL, 2, NULL);
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




/* Idle Task for sleeping the processor */
void vApplicationIdleHook(void) {
    /* Put the microcontroller in a low power mode */
    //SysCtlSleep();
		__ASM("wfi");
}