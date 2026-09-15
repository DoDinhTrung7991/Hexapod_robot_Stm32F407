#ifndef INC_SYSTICK_HEADER_H_
#define INC_SYSTICK_HEADER_H_

#include <stdint.h>

typedef struct SYSTICK_str
{
	volatile uint32_t CTRL;
	volatile uint32_t LOAD;
	volatile uint32_t VAL;
	volatile uint32_t CALIB;
} SYSTICK_t;

#define SYSTICK_reg ((SYSTICK_t *)0xE000E010UL)

#endif

