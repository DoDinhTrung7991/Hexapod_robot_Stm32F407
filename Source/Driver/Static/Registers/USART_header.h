#ifndef INC_USART_HEADER_H_
#define INC_USART_HEADER_H_

#include <stdint.h>

typedef struct USART_str
{
	volatile uint32_t SR;
	volatile uint32_t DR;
	volatile uint32_t BRR;
	volatile uint32_t CR1;
	volatile uint32_t CR2;
	volatile uint32_t CR3;
	volatile uint32_t GTPR;
} USART_t;

extern USART_t * const USART_reg[6];

#endif
