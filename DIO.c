#include "headers.h"

// Function to initialize the specified port
void DIO_Init(PortName port)
{
    SYSCTL_RCGCGPIO_R |= (ON << port);
    while ((SYSCTL_PRGPIO_R & (ON << port)) == OFF) {};

    volatile unsigned int *lock_reg;
    volatile unsigned int *cr_reg;
    switch (port) {
        case PORT_A: lock_reg = &GPIO_PORTA_LOCK_R; cr_reg = &GPIO_PORTA_CR_R; break;
        case PORT_B: lock_reg = &GPIO_PORTB_LOCK_R; cr_reg = &GPIO_PORTB_CR_R; break;
        case PORT_C: lock_reg = &GPIO_PORTC_LOCK_R; cr_reg = &GPIO_PORTC_CR_R; break;
        case PORT_D: lock_reg = &GPIO_PORTD_LOCK_R; cr_reg = &GPIO_PORTD_CR_R; break;
        case PORT_E: lock_reg = &GPIO_PORTE_LOCK_R; cr_reg = &GPIO_PORTE_CR_R; break;
        case PORT_F: lock_reg = &GPIO_PORTF_LOCK_R; cr_reg = &GPIO_PORTF_CR_R; break;
        default: return;
    }
    *lock_reg = LOCK; // Unlock register with the specific key
    *cr_reg = FF;         // Enable changes to all pins on the port
}


void DIO_Configure(PortName port, uint8 pin, uint8 digital, uint8 direction, uint8 resistorMode){
  volatile unsigned int *dir_reg;
    volatile unsigned int *den_reg;
    volatile unsigned int *pur_reg;
    volatile unsigned int *pdr_reg;

    switch (port) {
        case PORT_A: dir_reg = &GPIO_PORTA_DIR_R; den_reg = &GPIO_PORTA_DEN_R; pur_reg = &GPIO_PORTA_PUR_R; pdr_reg = &GPIO_PORTA_PDR_R; break;
        case PORT_B: dir_reg = &GPIO_PORTB_DIR_R; den_reg = &GPIO_PORTB_DEN_R; pur_reg = &GPIO_PORTB_PUR_R; pdr_reg = &GPIO_PORTB_PDR_R; break;
        case PORT_C: dir_reg = &GPIO_PORTC_DIR_R; den_reg = &GPIO_PORTC_DEN_R; pur_reg = &GPIO_PORTC_PUR_R; pdr_reg = &GPIO_PORTC_PDR_R; break;
        case PORT_D: dir_reg = &GPIO_PORTD_DIR_R; den_reg = &GPIO_PORTD_DEN_R; pur_reg = &GPIO_PORTD_PUR_R; pdr_reg = &GPIO_PORTD_PDR_R; break;
        case PORT_E: dir_reg = &GPIO_PORTE_DIR_R; den_reg = &GPIO_PORTE_DEN_R; pur_reg = &GPIO_PORTE_PUR_R; pdr_reg = &GPIO_PORTE_PDR_R; break;
        case PORT_F: dir_reg = &GPIO_PORTF_DIR_R; den_reg = &GPIO_PORTF_DEN_R; pur_reg = &GPIO_PORTF_PUR_R; pdr_reg = &GPIO_PORTF_PDR_R; break;
        default: return;
    }

    if (direction) {
        *dir_reg |= (ON << pin);  // Set pin as output (1 in data sheet)
    } else {
        *dir_reg &= ~(ON << pin); // Set pin as input (0 in data sheet)
    }

    *den_reg |= (ON << pin); // Enable digital function

    if (resistorMode == PUR) {
        *pur_reg |= (ON << pin); // Enable pull-up resistor
    } else if (resistorMode == PDR) {
        *pdr_reg |= (ON << pin); // Enable pull-down resistor
    }
}



// Function to write to a specific pin on a port
void DIO_WritePin(PortName port, unsigned int pin, unsigned int value)
{
    volatile unsigned int *data_reg;
    switch (port) {
        case PORT_A: data_reg = &GPIO_PORTA_DATA_R; break;
        case PORT_B: data_reg = &GPIO_PORTB_DATA_R; break;
        case PORT_C: data_reg = &GPIO_PORTC_DATA_R; break;
        case PORT_D: data_reg = &GPIO_PORTD_DATA_R; break;
        case PORT_E: data_reg = &GPIO_PORTE_DATA_R; break;
        case PORT_F: data_reg = &GPIO_PORTF_DATA_R; break;
        default: return;
    }

    if (value) {
        *data_reg |= (ON << pin);  // Set pin to 1
    } else {
        *data_reg &= ~(ON << pin); // Clear pin to 0
    }
}

// Function to write an 8-bit value to an entire port
void DIO_WritePort(PortName port, unsigned int value)
{
    volatile unsigned int *data_reg;
    switch (port) {
        case PORT_A: data_reg = &GPIO_PORTA_DATA_R; break;
        case PORT_B: data_reg = &GPIO_PORTB_DATA_R; break;
        case PORT_C: data_reg = &GPIO_PORTC_DATA_R; break;
        case PORT_D: data_reg = &GPIO_PORTD_DATA_R; break;
        case PORT_E: data_reg = &GPIO_PORTE_DATA_R; break;
        case PORT_F: data_reg = &GPIO_PORTF_DATA_R; break;
        default: return;
    }

    *data_reg = value & FF; // Write only the lower 8 bits
}
