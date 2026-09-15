#ifndef INC_IWDG_HEADER_H_
#define INC_IWDG_HEADER_H_

#include <stdint.h>

typedef struct IWDG_str
{
    volatile uint32_t KR;
    volatile uint32_t PR;
    volatile uint32_t RLR;
    volatile uint32_t SR;
} IWDG_t;

#define IWDG_reg ((IWDG_t *)0x40003000UL)

#endif
