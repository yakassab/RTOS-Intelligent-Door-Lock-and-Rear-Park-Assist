#ifndef HEADERS_H
#define HEADERS_H

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include "tm4c123gh6pm.h"
#include "types.h"
#include "bitwise_operation.h"
#include "TM4C123.h"

#include "LCD.h"
#include "DIO.h"
#include "potentiometer.h"
#include "ignition.h"
#include "gear.h"
#include "ultrasonic.h"
#include "buzzer.h"
#include "door.h"





#include <FreeRTOS.h>
#include "task.h"

#define PORTF_ON 1<<5U

#define RED_LED 1<<1U
#define BLUE_LED 1<<2U
#define GREEN_LED 1<<3U
#define WHITE_LED 0x0E
#define BLACK_LED 0x00

#define RED_PIN 1U
#define BLUE_PIN 2U
#define GREEN_PIN 3U

#define STATE_0 0
#define STATE_1 1
#define STATE_2 2
#define STATE_3 3

#define ON 1U
#define OFF 0U

#define MILLION 1000000U
#define HUNDRED 100U
#define EIGHT 8U
#define SIX 6U
#define FIVE 5U
#define FOUR 4U
#define THREE 3U
#define TWO 2U
#define ONE 1U
#define ZERO 0U
//#define NULL '\0'

#define ZERO_FLOAT 0.0

#define LED_RED   (1<<1)  // PF1
#define LED_BLUE  (1<<2)  // PF2
#define LED_GREEN (1<<3)  // PF3


#define INPUT 0U
#define OUTPUT 1U

#define DIGITAL 1U

#define PUR 1U
#define PDR 2U

#define LOCK 0x4C4F434B

#define FF 0xFF

#define LM35_CHANNEL    0x01  // LM35 connected to AIN1 (PE2)
#define VREF            3.28  // Update this with measured VREF (e.g., 3.28V)
#define ADC_RESOLUTION  4096  // 12-bit ADC resolution

#define MAX_TEMPERATURE 20U

typedef enum {
    PIN_ZERO = 0U,
    PIN_ONE,
    PIN_TWO,
    PIN_THREE,
    PIN_FOUR,
    PIN_FIVE,
    PIN_SIX,
    PIN_SEVEN
} PinName;

extern TaskHandle_t UltrasonicHandle;

#endif