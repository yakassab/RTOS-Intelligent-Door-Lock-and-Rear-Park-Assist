#ifndef IGNITION_H
#define IGNITION_H

#include "headers.h"


extern bool ignition;
extern bool ignition_changed;

void check_ignition(void);
void IgnitionCheckTask(void *pvParameters);


#endif