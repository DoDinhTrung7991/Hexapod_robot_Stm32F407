/*
 * RCC_header.h
 *
 *  Created on: Aug 24, 2022
 *      Author: trung97
 */

#ifndef INC_RCC_HEADER_H_
#define INC_RCC_HEADER_H_

#include <stdint.h>

typedef struct RCC_str
{
	volatile uint32_t CR;
	volatile uint32_t PLLCFGR;
	volatile uint32_t CFGR;
	volatile uint32_t CIR;
	volatile uint32_t AHB1RSTR;
	volatile uint32_t AHB2RSTR;
	volatile uint32_t AHB3RSTR;
	uint32_t Reserved_0;
	volatile uint32_t APB1RSTR;
	volatile uint32_t APB2RSTR;
	uint32_t Reserved_1;
	uint32_t Reserved_2;
	volatile uint32_t AHB1ENR;
	volatile uint32_t AHB2ENR;
	volatile uint32_t AHB3ENR;
	uint32_t Reserved_3;
	volatile uint32_t APB1ENR;
	volatile uint32_t APB2ENR;
	uint32_t Reserved_4;
	uint32_t Reserved_5;
	volatile uint32_t AHB1LPENR;
	volatile uint32_t AHB2LPENR;
	volatile uint32_t AHB3LPENR;
	uint32_t Reserved_6;
	volatile uint32_t APB1LPENR;
	volatile uint32_t APB2LPENR;
	uint32_t Reserved_7;
	uint32_t Reserved_8;
	volatile uint32_t BDCR;
	volatile uint32_t CSR;
	uint32_t Reserved_9;
	uint32_t Reserved_10;
	volatile uint32_t SSCGR;
	volatile uint32_t PLLI2SCFGR;
} RCC_t;

#define RCC_reg ((RCC_t *)0x40023800UL)

#endif /* INC_RCC_HEADER_H_ */
