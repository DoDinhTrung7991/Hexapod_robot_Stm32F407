#ifndef INC_WWDG_HEADER_H_
#define INC_WWDG_HEADER_H_

#include <stdint.h>

typedef struct WWDG_str
{
    volatile uint32_t CR;
    volatile uint32_t CFR;
    volatile uint32_t SR;
} WWDG_t;

#define WWDG_reg ((WWDG_t *)0x40002C00UL)

#endif
