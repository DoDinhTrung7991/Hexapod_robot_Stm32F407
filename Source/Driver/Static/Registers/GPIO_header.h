/*
 * GPIO_header.h
 *
 *  Created on: Aug 23, 2022
 *      Author: trung97
 */

#ifndef INC_GPIO_HEADER_H_
#define INC_GPIO_HEADER_H_

#include <stdint.h>

typedef struct GPIOx_str
{
	volatile uint32_t MODER;
	volatile uint32_t OTYPER;
	volatile uint32_t OSPEEDR;
	volatile uint32_t PUPDR;
	volatile uint32_t IDR;
	volatile uint32_t ODR;
	volatile uint32_t BSRR;
	volatile uint32_t LCKR;
	volatile uint32_t AFRL;
	volatile uint32_t AFRH;
} GPIOx_t;

extern GPIOx_t * const GPIO_reg[9];

#endif /* INC_GPIO_HEADER_H_ */

