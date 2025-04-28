#ifndef POTENTIOMETER_H
#define POTENTIOMETER_H

#include <stdint.h>
#include <stdbool.h>

// Initializes ADC and creates the FreeRTOS task for speed sampling
void Potentiometer_Init(void);

// Gets the latest speed value (in km/h)
float Potentiometer_GetSpeed(void);

#endif // POTENTIOMETER_H 