#ifndef DOOR_LOCK_H
#define DOOR_LOCK_H

#include <stdint.h>
#include <stdbool.h>
#include "FreeRTOS.h"
#include "task.h"

typedef enum {
    DOOR_LOCKED,
    DOOR_UNLOCKED
} DoorState_t;

// Initializes GPIO and creates the FreeRTOS task for manual lock/unlock
void DoorLock_Init(void);

// Returns the current door state
DoorState_t DoorLock_GetState(void);

#endif // DOOR_LOCK_H 