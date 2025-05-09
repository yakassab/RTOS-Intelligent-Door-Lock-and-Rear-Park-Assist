#ifndef DOOR_H
#define DOOR_H

#include "headers.h"


extern bool door_opened;
extern bool door_locked;
extern bool lock_changed;
extern bool door_changed;

extern bool manual_override;

extern bool cant_unlock_door;

extern bool lock_state;
extern bool door_state;

extern SemaphoreHandle_t xDoorSemaphore;
extern SemaphoreHandle_t xLockSemaphore;

void DoorCheckTask(void *pvParameters);
void DoorLockTask(void *pvParameters);



#endif