#ifndef ULTRASONIC_H
#define ULTRASONIC_H

#include <stdint.h>
#include <stdbool.h>

extern uint32_t latest_distance;

// Initializes GPIO, timer, and creates the FreeRTOS task for distance measurement
void Ultrasonic_Init(void);

// Gets the latest measured distance (in cm)
uint32_t Ultrasonic_GetDistance(void);

#endif 