#include "WWDG_header.h"
#include "RCC_header.h"
#include "WWDG.h"
#include "bit_operator.h"
#include "Interrupt.h"
#include "init_static.h"

const uint32_t WWDG_prescaler_u8[4] = {1, 2, 4, 8};

bool WWDG_init(WWDG_prescaler_t prescaler, uint8_t timeoutMS_u8, uint8_t earlyFail_thresholdMS_u8)
{
    uint32_t temp_u32;

    temp_u32 = (((uint32_t)timeoutMS_u8 * APB1_freq) / (4096UL * 1000UL * (uint32_t)WWDG_prescaler_u8[prescaler])) - 1UL;

    if (0x3FUL < temp_u32)
    {
        return NOT_OK;
    }

    // Enable WWDG clock
    SET_BIT(RCC_reg->APB1ENR, 11U);
    // Set prescaler
    WRITE_REG(WWDG_reg->CFR, 0x3UL, 7U, prescaler);
    // Set bit T6
    SET_BIT(WWDG_reg->CR, 6U);
    // Write T[5:0]
    WRITE_REG(WWDG_reg->CR, 0x3FUL, 0U, (uint32_t)temp_u32);
    temp_u32 = temp_u32 - ((earlyFail_thresholdMS_u8 * APB1_freq) / (4096UL * 1000UL * (uint32_t)WWDG_prescaler_u8[prescaler]));
    // Write W[6:0]
    WRITE_REG(WWDG_reg->CFR, 0x7FUL, 0U, (uint32_t)temp_u32);
    // Set bit EWI
    SET_BIT(WWDG_reg->CFR, 9U);
    // Enable Interrupt
	NVIC_ISER_setVal(WWDG);
    // Enable WWDG
    SET_BIT(WWDG_reg->CR, 7U);

    return OK;
}

void WWDG_reset(void)
{
    // Set bit T6
    SET_BIT(WWDG_reg->CR, 6U);
}
