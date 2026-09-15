#include "IWDG_header.h"
#include "IWDG.h"
#include "bit_operator.h"
#include "Interrupt.h"

#define WATCHDOG_START 0xCCCCUL
#define WATCHDOG_RESET 0xAAAAUL
#define WATCHDOG_WR_ENABLE 0x5555UL

bool IWDG_init(IWDG_timeout_t timeout)
{
    uint32_t timeStart_u32;

    // Start IWDG
    WRITE_REG(IWDG_reg->KR, 0xFFFFUL, 0U, WATCHDOG_START);
    // Enable write IWDG_PR and IWDG_RLR
    WRITE_REG(IWDG_reg->KR, 0xFFFFUL, 0U, WATCHDOG_WR_ENABLE);
    // Setting Prescaler
    WRITE_REG(IWDG_reg->PR, 0x7UL, 0U, timeout);

    timeStart_u32 = SysTick_cnt_u32;

    // Waiting for Prescaler update completed.
    while (READ_REG(IWDG_reg->SR, 1UL, 0U))
    {
        if (
			((SysTick_cnt_u32 < timeStart_u32) && (5UL <= (SysTick_cnt_u32 + (0xFFFFFFFFUL - timeStart_u32) + 1)))
			|| ((SysTick_cnt_u32 >= timeStart_u32) && (5UL <= (SysTick_cnt_u32 - timeStart_u32)))
		)
		{
			return NOT_OK;
		}
    }

    // Reset IWDG Counter
    WRITE_REG(IWDG_reg->KR, 0xFFFFUL, 0U, WATCHDOG_RESET);

    return OK;
}

void IWDG_reset(void)
{
    // Reset IWDG Counter
    WRITE_REG(IWDG_reg->KR, 0xFFFFUL, 0U, WATCHDOG_RESET);
}
