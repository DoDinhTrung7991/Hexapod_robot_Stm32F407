/*
 * PWR_header.h
 *
 *  Created on: Aug 26, 2022
 *      Author: trung97
 */

#ifndef INC_PWR_HEADER_H_
#define INC_PWR_HEADER_H_

#include <stdint.h>

typedef struct PWR_str
{
	volatile uint32_t CR;
	volatile uint32_t CSR;
} PWR_t;

#define PWR_reg ((PWR_t *)0x40007000UL)

#endif /* INC_PWR_HEADER_H_ */
