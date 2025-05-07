//#include "potentiometer.h"
//#include "tm4c123gh6pm.h"
//#include "FreeRTOS.h"
//#include "task.h"

//#define SPEED_ADC_CHANNEL 1 // PE2 (AIN1)
//#define SPEED_TASK_STACK 128
//#define SPEED_TASK_PRIORITY 2

//static float latest_speed = 0.0f;

//static void ADC0_Init(void);
//static uint32_t ADC0_Read(void);
//static void vSpeedTask(void *pvParameters);

//void Potentiometer_Init(void) {
//    ADC0_Init();
//    xTaskCreate(vSpeedTask, "SpeedTask", SPEED_TASK_STACK, NULL, SPEED_TASK_PRIORITY, NULL);
//}

//float Potentiometer_GetSpeed(void) {
//    return latest_speed;
//}

//static void ADC0_Init(void) {
//    SYSCTL_RCGCGPIO_R |= (1 << 4);       // Enable clock for PORTE
//    SYSCTL_RCGCADC_R |= 1;               // Enable clock for ADC0

//    GPIO_PORTE_AFSEL_R |= (1 << 2);      // Enable alternate function on PE2
//    GPIO_PORTE_DEN_R &= ~(1 << 2);       // Disable digital on PE2
//    GPIO_PORTE_AMSEL_R |= (1 << 2);      // Enable analog mode on PE2

//    ADC0_ACTSS_R &= ~8;                  // Disable SS3
//    ADC0_EMUX_R &= ~0xF000;              // Software trigger
//    ADC0_SSMUX3_R = SPEED_ADC_CHANNEL;   // Select AIN1 (PE2)
//    ADC0_SSCTL3_R = 0x06;                // End of sequence, INT flag
//    ADC0_ACTSS_R |= 8;                   // Enable SS3
//}

//static uint32_t ADC0_Read(void) {
//    ADC0_PSSI_R = 8;                     // Start SS3
//    while ((ADC0_RIS_R & 8) == 0);       // Wait for conversion
//    uint32_t result = ADC0_SSFIFO3_R & 0xFFF;
//    ADC0_ISC_R = 8;                      // Clear completion flag
//    return result;
//}



//==================================================

#include "headers.h"

#define SPEED_ADC_CHANNEL 1 // PE2 (AIN1)

float speed = 0;

bool speed_lock = false;

static void ADC0_Init(void);
static uint32_t ADC0_Read(void);

void Potentiometer_Init(void) {
    ADC0_Init();
}

float Potentiometer_GetSpeed(void) {
    uint32_t adc_value = ADC0_Read();
    float voltage = (adc_value * 3.3f) / 4095.0f;
    return voltage * 20.0f; // Simulate speed in km/h
}

static void ADC0_Init(void) {
    SYSCTL_RCGCGPIO_R |= (1 << 4);       // Enable clock for PORTE
    SYSCTL_RCGCADC_R |= 1;               // Enable clock for ADC0

    GPIO_PORTE_AFSEL_R |= (1 << 2);      // Enable alternate function on PE2
    GPIO_PORTE_DEN_R &= ~(1 << 2);       // Disable digital on PE2
    GPIO_PORTE_AMSEL_R |= (1 << 2);      // Enable analog mode on PE2

    ADC0_ACTSS_R &= ~8;                  // Disable SS3
    ADC0_EMUX_R &= ~0xF000;              // Software trigger
    ADC0_SSMUX3_R = SPEED_ADC_CHANNEL;   // Select AIN1 (PE2)
    ADC0_SSCTL3_R = 0x06;                // End of sequence, INT flag
    ADC0_ACTSS_R |= 8;                   // Enable SS3
}

static uint32_t ADC0_Read(void) {
    ADC0_PSSI_R = 8;                     // Start SS3
    while ((ADC0_RIS_R & 8) == 0);       // Wait for conversion
    uint32_t result = ADC0_SSFIFO3_R & 0xFFF;
    ADC0_ISC_R = 8;                      // Clear completion flag
    return result;
}

void vSpeedTask(void *pvParameters) {
    (void)pvParameters;
	TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xFrequency = pdMS_TO_TICKS(30); // Check every 20ms
    for (;;) {
        uint32_t adc_value = ADC0_Read();
        float voltage = (adc_value * 3.3f) / 4095.0f;
        speed = voltage * 20.0f; // Simulate speed in km/h
				if (speed > 20 && !manual_override){speed_lock = true;}
				
				
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
} 

