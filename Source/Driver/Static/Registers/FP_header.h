#ifndef INC_FP_HEADER_H_
#define INC_FP_HEADER_H_

#include <stdint.h>

typedef struct FP_str
{
	volatile uint32_t FPCCR;    // 0xE000EF34
	volatile uint32_t FPCAR;    // 0xE000EF38
	volatile uint32_t FPDSCR;   // 0xE000EF3C
} FP_t;

#define FP_reg ((FP_t *)0xE000EF34UL)

#endif /* INC_FP_HEADER_H_ */
