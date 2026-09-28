#include "Interrupt.h"
#include "SYSCFG_header.h"
#include "NVIC_header.h"
#include "EXTI_header.h"
#include "bit_operator.h"

void NVIC_ISER_setVal(peripheral_Selection_t peripheral_Selection_en)
{
    uint8_t bytePos = peripheral_Selection_en / 32U;
    uint8_t bitPos = peripheral_Selection_en - (bytePos * 32U);
    SET_BIT(NVIC_reg->ISER[bytePos], bitPos);
}

void NVIC_ICER_setVal(peripheral_Selection_t peripheral_Selection_en)
{
    uint8_t bytePos = peripheral_Selection_en / 32U;
    uint8_t bitPos = peripheral_Selection_en - (bytePos * 32U);
    SET_BIT(NVIC_reg->ICER[bytePos], bitPos);
}

bool Ex_Interrupt(GPIO_ENABLE_t GPIOx_en, uint8_t pos_u8, GPIO_pullDir_t GPIO_pullDir_en, EvRaising_Dir_t EvRaising_Dir_en)
{
    bool returnVal = OK;
    uint8_t EXTICR_i_u8 = 0;
    uint8_t EXTICR_pos_u8 = 0;

    if ((15U < pos_u8) || (GPIOAEN > GPIOx_en) || (GPIOIEN < GPIOx_en))
	{
		returnVal = NOT_OK;
		return returnVal;
	}

    switch (EvRaising_Dir_en)
    {
        case Falling_Edge:
            CLEAR_BIT(EXTI_reg->RTSR, pos_u8);
            SET_BIT(EXTI_reg->FTSR, pos_u8);
            break;

        case Rising_Edge:
            SET_BIT(EXTI_reg->RTSR, pos_u8);
            CLEAR_BIT(EXTI_reg->FTSR, pos_u8);
            break;

        case Both_Edge:
            SET_BIT(EXTI_reg->RTSR, pos_u8);
            SET_BIT(EXTI_reg->FTSR, pos_u8);
            break;

        default:
            returnVal = NOT_OK;
            break;
    }

    CLEAR_BIT(EXTI_reg->EMR, pos_u8);
    SET_BIT(EXTI_reg->IMR, pos_u8);

    // This line must be in your external interrupt initialization function (e.g., Ex_Interrupt)
    SET_BIT(RCC_reg->APB2ENR, 14U); // Enable SYSCFG clock

    EXTICR_i_u8 = pos_u8 / 4U;
    EXTICR_pos_u8 = (pos_u8 - (EXTICR_i_u8 * 4U)) * 4U;
    WRITE_REG(SYSCFG_reg->EXTICR[EXTICR_i_u8], 0xFUL, EXTICR_pos_u8, GPIOx_en);

    switch (pos_u8)
    {
        case 0:
            NVIC_ISER_setVal(EXTI0);
            break;

        case 1:
            NVIC_ISER_setVal(EXTI1);
            break;

        case 2:
            NVIC_ISER_setVal(EXTI2);
            break;

        case 3:
            NVIC_ISER_setVal(EXTI3);
            break;

        case 4:
            NVIC_ISER_setVal(EXTI4);
            break;

        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
            NVIC_ISER_setVal(EXTI9_5);
            break;

        case 10:
        case 11:
        case 12:
        case 13:
        case 14:
        case 15:
            NVIC_ISER_setVal(EXTI15_10);
            break;

        default:
            returnVal = NOT_OK;
            break;
    }

    return returnVal;
}
