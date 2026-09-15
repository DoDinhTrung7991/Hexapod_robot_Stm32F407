#ifndef INC_EXTI_HEADER_H_
#define INC_EXTI_HEADER_H_

#include <stdint.h>

typedef struct EXTI_str
{
	volatile uint32_t IMR;
	volatile uint32_t EMR;
	volatile uint32_t RTSR;
	volatile uint32_t FTSR;
	volatile uint32_t SWIER;
	volatile uint32_t PR;
} EXTI_t;

#define EXTI_reg ((EXTI_t*)0x40013C00UL)

#endif

