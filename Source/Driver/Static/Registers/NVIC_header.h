#ifndef INC_NVIC_HEADER_H_
#define INC_NVIC_HEADER_H_

#include <stdint.h>

typedef struct NVIC_str
{
	volatile uint32_t ISER[8U];
	volatile uint32_t Reserve0[24U];
	volatile uint32_t ICER[8U];
	volatile uint32_t Reserve1[24U];
	volatile uint32_t ISPR[8U];
	volatile uint32_t Reserve2[24U];
	volatile uint32_t ICPR[8U];
	volatile uint32_t Reserve3[24U];
	volatile uint32_t IABR[8U];
	volatile uint32_t Reserve4[56U];
	volatile uint32_t IPR[60U];
	volatile uint32_t Reserve5[644U];
	volatile uint32_t STIR;
} NVIC_t;

#define NVIC_reg ((NVIC_t*)0xE000E100UL)

#endif
