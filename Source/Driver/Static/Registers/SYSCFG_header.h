#ifndef INC_SYSCFG_HEADER_H_
#define INC_SYSCFG_HEADER_H_

#include <stdint.h>

typedef struct SYSCFG_str
{
	volatile uint32_t MEMRMP;
	volatile uint32_t PMC;
	volatile uint32_t EXTICR[4];
	volatile uint32_t CMPCR;
} SYSCFG_t;

#define SYSCFG_reg ((SYSCFG_t*)0x40013800UL)

#endif

