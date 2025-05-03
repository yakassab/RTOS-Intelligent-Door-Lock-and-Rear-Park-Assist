#ifndef BUZZER_H
#define BUZZER_H

#include "headers.h"

extern bool buzz;

void check_buzz(void);

 void vBuzzerTask(void *pvParameters);

#endif