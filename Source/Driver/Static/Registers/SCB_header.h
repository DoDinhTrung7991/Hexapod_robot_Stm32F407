/*
 * SCB_header.h
 *
 *  Created on: Aug 25, 2022
 *      Author: trung97
 */

#ifndef INC_SCB_HEADER_H_
#define INC_SCB_HEADER_H_

#include <stdint.h>

typedef struct SCB_str
{
	volatile uint32_t CPUID;
	volatile uint32_t ICSR;
	volatile uint32_t VTOR;
	volatile uint32_t AIRCR;
	volatile uint32_t SCR;
	volatile uint32_t CCR;
	volatile uint32_t SHPR1;
	volatile uint32_t SHPR2;
	volatile uint32_t SHPR3;
	volatile uint32_t SHCSR;
	volatile uint32_t CFSR;
	volatile uint32_t HFSR;
	volatile uint32_t DFSR; // RESERVED_DEBUG
	volatile uint32_t MMAR;
	volatile uint32_t BFAR;
	volatile uint32_t AFSR;
	uint32_t Reserve0[18];
	volatile uint32_t CPACR;    // 0xE000ED88
} SCB_t;

#define SCB_reg ((SCB_t *)0xE000ED00UL)

#endif /* INC_SCB_HEADER_H_ */
