#ifndef BUZZER_H
#define BUZZER_H

#include "headers.h"

extern bool buzz;

void check_buzz(void);

void vBuzzerTask(void *pvParameters);
 
void RGB_LED_Off(void);
 
void RGB_LED_Set(uint8_t red, uint8_t green, uint8_t blue);

void RGB_LED_Red(void);
void RGB_LED_Green(void);
void RGB_LED_Yellow(void);

#endif