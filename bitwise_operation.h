#include "headers.h"

#define SET_BIT(Register, Bit)  ((Register) |= (ON << (Bit)))

#define CLEAR_BIT(Register, Bit)  ((Register) &= ~(ON << (Bit)))

#define GET_BIT(Register, Bit)  (((Register) >> (Bit)) & ON)

#define TOGGLE_BIT(Register, Bit) (((Register)) ^= (ON << (Bit)))