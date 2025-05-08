#ifndef ULTRASONIC_H
#define ULTRASONIC_H

#include "headers.h"

extern uint32_t latest_distance;

extern TaskHandle_t UltrasonicHandle;
extern SemaphoreHandle_t xUltrasonicSemaphore;

extern float distance;


// Initializes GPIO, timer, and creates the FreeRTOS task for distance measurement
void Ultrasonic_Init(void);

// Gets the latest measured distance (in cm)
uint32_t Ultrasonic_GetDistance(void);

void vUltrasonicTask(void *pvParameters);

#endif 