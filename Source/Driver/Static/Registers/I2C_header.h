#ifndef INC_I2C_HEADER_H_
#define INC_I2C_HEADER_H_

#include <stdint.h>

typedef struct I2C_str
{
    volatile uint32_t CR1;
    volatile uint32_t CR2;
    volatile uint32_t OAR1;
    volatile uint32_t OAR2;
    volatile uint32_t DR;
    volatile uint32_t SR1;
    volatile uint32_t SR2;
    volatile uint32_t CCR;
    volatile uint32_t TRISE;
    volatile uint32_t FLTR;
} I2C_t;

extern I2C_t * const I2C_reg[3];

#endif
