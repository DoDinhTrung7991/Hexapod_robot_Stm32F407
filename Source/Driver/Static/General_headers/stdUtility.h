#ifndef _STDUTILITY_H_
#define _STDUTILITY_H_

#include <stdint.h>
#include <stdbool.h>

#define NULL (void*)0

/* Data Memory Barrier: Đảm bảo thứ tự truy cập bộ nhớ */
#define __DMB()    __asm__ volatile ("dmb 0xF" ::: "memory")

/* Data Synchronization Barrier: Chờ tất cả bus transfer hoàn tất */
#define __DSB()    __asm__ volatile ("dsb 0xF" ::: "memory")

/* Instruction Synchronization Barrier: Xả pipeline chỉ lệnh */
#define __ISB()    __asm__ volatile ("isb 0xF" ::: "memory")

#define ENABLE 1
#define DISABLE 0

#define OK 0U
#define NOT_OK 1U

#define INITTED 1
#define NOT_INITTED 0

#define FAIL -1

typedef struct
{
	volatile uint32_t *reg_u32_ptr;
	unsigned int pos;
} RCC_APBxENR_enable_t;

#endif

