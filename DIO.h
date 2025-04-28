#ifndef DIO_H
#define DIO_H

#include "headers.h"


typedef enum {
    PORT_A = 0,
    PORT_B = 1,
    PORT_C = 2,
    PORT_D = 3,
    PORT_E = 4,
    PORT_F = 5
} PortName;

void DIO_Init(PortName port);

void DIO_Configure(PortName port, uint8 pin, uint8 digital, uint8 direction, uint8 resistorMode);

void DIO_WritePin(PortName port, unsigned int pin, unsigned int value);

void DIO_WritePort(PortName port, unsigned int value);

#endif