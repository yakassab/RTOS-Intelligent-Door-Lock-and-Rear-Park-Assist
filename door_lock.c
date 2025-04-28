#include "door_lock.h"
#include "TM4C123.h"
#include "basic_io.h" // For vPrintString, if you want serial output

#define MANUAL_BUTTON_PIN   (1U << 0) // PF0
#define DOOR_LOCK_PIN       (1U << 2) // PF2
#define DRIVER_DOOR_PIN     (1U << 1) // PF1
#define IGNITION_PIN        (1U << 3) // PF3
#define POTENTIOMETER_ADC_CH 0         // AIN0 (PE3)
#define SPEED_THRESHOLD_KMH  10

static volatile DoorState_t doorState = DOOR_UNLOCKED;
static TaskHandle_t xDoorTaskHandle = NULL;

static void GPIO_DoorLockInit(void);
static void ADC_PotentiometerInit(void);
static uint16_t ADC_ReadPotentiometer(void);
static void SetDoorLock(DoorState_t state);
static void vDoorTask(void *pvParameters);

void DoorLock_Init(void)
{
    GPIO_DoorLockInit();
    ADC_PotentiometerInit();
    xTaskCreate(vDoorTask, "DoorTask", 256, NULL, 3, &xDoorTaskHandle);
}

DoorState_t DoorLock_GetState(void)
{
    return doorState;
}

static void GPIO_DoorLockInit(void)
{
    SYSCTL->RCGCGPIO |= (1U << 5); // Port F
    while ((SYSCTL->PRGPIO & (1U << 5)) == 0) {}

    GPIOF->LOCK = 0x4C4F434B;
    GPIOF->CR |= MANUAL_BUTTON_PIN | DOOR_LOCK_PIN | DRIVER_DOOR_PIN | IGNITION_PIN;
    GPIOF->DIR &= ~(MANUAL_BUTTON_PIN | DRIVER_DOOR_PIN | IGNITION_PIN); // Inputs
    GPIOF->DIR |= DOOR_LOCK_PIN; // Output
    GPIOF->PUR |= MANUAL_BUTTON_PIN | DRIVER_DOOR_PIN | IGNITION_PIN;
    GPIOF->DEN |= MANUAL_BUTTON_PIN | DOOR_LOCK_PIN | DRIVER_DOOR_PIN | IGNITION_PIN;
}

static void ADC_PotentiometerInit(void)
{
    SYSCTL->RCGCADC |= 1; // Enable ADC0
    SYSCTL->RCGCGPIO |= (1U << 4); // Port E
    while ((SYSCTL->PRGPIO & (1U << 4)) == 0) {}
    GPIOE->DIR &= ~(1U << 3); // PE3 input
    GPIOE->AFSEL |= (1U << 3);
    GPIOE->DEN &= ~(1U << 3);
    GPIOE->AMSEL |= (1U << 3);

    ADC0->ACTSS &= ~8;
    ADC0->EMUX &= ~0xF000;
    ADC0->SSMUX3 = POTENTIOMETER_ADC_CH;
    ADC0->SSCTL3 = 0x6;
    ADC0->ACTSS |= 8;
}

static uint16_t ADC_ReadPotentiometer(void)
{
    ADC0->PSSI = 8;
    while ((ADC0->RIS & 8) == 0) {}
    uint16_t result = ADC0->SSFIFO3 & 0xFFF;
    ADC0->ISC = 8;
    return result;
}

static void SetDoorLock(DoorState_t state)
{
    if (state == DOOR_LOCKED) {
        GPIOF->DATA |= DOOR_LOCK_PIN;
    } else {
        GPIOF->DATA &= ~DOOR_LOCK_PIN;
    }
    doorState = state;
}

static void vDoorTask(void *pvParameters)
{
    uint8_t prevBtn = 1;
    uint8_t prevIgnition = 1;
    for (;;) {
        // Manual override (toggle)
        uint8_t btn = (GPIOF->DATA & MANUAL_BUTTON_PIN) ? 1 : 0;
        if (!btn && prevBtn) {
            if (doorState == DOOR_UNLOCKED) {
                SetDoorLock(DOOR_LOCKED);
                vPrintString("Doors Locked (Manual Override)\n");
            } else {
                SetDoorLock(DOOR_UNLOCKED);
                vPrintString("Doors Unlocked (Manual Override)\n");
            }
        }
        prevBtn = btn;

        // Ignition switch logic
        uint8_t ignition = (GPIOF->DATA & IGNITION_PIN) ? 1 : 0;
        if (!ignition && prevIgnition) SetDoorLock(DOOR_UNLOCKED); // Ignition turned off
        prevIgnition = ignition;

        // Potentiometer speed logic
        uint16_t adc = ADC_ReadPotentiometer();
        float speed = (adc / 4095.0f) * 100.0f; // Map to 0-100 km/h (example)
        if (speed > SPEED_THRESHOLD_KMH) SetDoorLock(DOOR_LOCKED);

        // Driver door switch (optional: can be used for alerts, not lock logic)
        uint8_t driverDoor = (GPIOF->DATA & DRIVER_DOOR_PIN) ? 1 : 0;
        // You can add logic here for alerts if needed

        vTaskDelay(pdMS_TO_TICKS(50));
    }
} 