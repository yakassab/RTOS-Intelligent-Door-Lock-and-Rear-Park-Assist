//#ifndef POTENTIOMETER_H
//#define POTENTIOMETER_H

//#include <stdint.h>
//#include <stdbool.h>

//// Initializes ADC and creates the FreeRTOS task for speed sampling
//void Potentiometer_Init(void);

//// Gets the latest speed value (in km/h)
//float Potentiometer_GetSpeed(void);

//#endif // POTENTIOMETER_H 

#ifndef POTENTIOMETER_H
#define POTENTIOMETER_H

#include "headers.h"


extern float speed;
extern bool speed_lock;

// Initializes ADC for potentiometer reading
void vSpeedTask(void *pvParameters);
void Potentiometer_Init(void);

// Gets the speed value (in km/h) directly from potentiometer
float Potentiometer_GetSpeed(void);

#endif // POTENTIOMETER_H