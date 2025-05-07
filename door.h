#ifndef DOOR_H
#define DOOR_H

#include "headers.h"


extern bool door_opened;
extern bool door_locked;

extern bool manual_override;


void DoorCheckTask(void *pvParameters);



#endif