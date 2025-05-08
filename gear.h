#ifndef GEAR_H
#define GEAR_H

#include "headers.h"

extern bool drive;

extern bool gear_state;

extern SemaphoreHandle_t xGearSemaphore;


void GearCheckTask(void *pvParameters);




#endif