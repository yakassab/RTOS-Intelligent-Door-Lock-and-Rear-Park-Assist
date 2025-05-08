#ifndef IGNITION_H
#define IGNITION_H

#include "headers.h"
#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>

// Declare variables and constants
#define IGNITION_ON    1
#define IGNITION_OFF   0

extern bool ignition;
extern bool ignition_changed;
extern bool ignition_state;
extern SemaphoreHandle_t xIgnitionSemaphore;


// Function prototypes
void check_ignition(void);
void IgnitionCheckTask(void *pvParameters);

#endif