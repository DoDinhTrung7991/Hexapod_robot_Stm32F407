/*
 * FLASH_header.h
 *
 *  Created on: Aug 25, 2022
 *      Author: trung97
 */

#ifndef INC_FLASH_HEADER_H_
#define INC_FLASH_HEADER_H_

#include <stdint.h>

typedef struct FLASH_str
{
	volatile uint32_t ACR;
	volatile uint32_t KEYR;
	volatile uint32_t OPTKEYR;
	volatile uint32_t SR;
	volatile uint32_t CR;
	volatile uint32_t OPTCR;
} FLASH_t;

#define FLASH_reg ((FLASH_t *)0x40023C00UL)

#endif /* INC_FLASH_HEADER_H_ */
