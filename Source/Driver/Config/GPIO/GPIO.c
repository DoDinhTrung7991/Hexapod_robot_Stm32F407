#include "GPIO.h"
#include "bit_operator.h"

GPIOx_t * const GPIO_reg[9] = {
	(GPIOx_t *)0x40020000,
	(GPIOx_t *)0x40020400,
	(GPIOx_t *)0x40020800,
	(GPIOx_t *)0x40020C00,
	(GPIOx_t *)0x40021000,
	(GPIOx_t *)0x40021400,
	(GPIOx_t *)0x40021800,
	(GPIOx_t *)0x40021C00,
	(GPIOx_t *)0x40022000
};

static void GPIO_init(GPIO_ENABLE_t GPIOx_en);

bool GPIO_setup(
	GPIO_ENABLE_t GPIOx_en, uint8_t pos_u8, GPIO_purpose_t GPIO_purpose_en, 
	AFRx_t AFx_en, GPIO_outputType_t GPIO_outputType_en, GPIO_pullDir_t GPIO_pullDir_en
)
{
	bool returnVal = OK;

	if ((15U < pos_u8) || (GPIOAEN > GPIOx_en) || (GPIOIEN < GPIOx_en))
	{
		returnVal = NOT_OK;
		return returnVal;
	}
	else
	{
		GPIO_init(GPIOx_en);

		switch (GPIO_purpose_en)
		{
			case IN:
				WRITE_REG(GPIO_reg[GPIOx_en]->MODER, 3UL, (pos_u8 * 2U), 0UL);
				break;

			case GP_OUT:
				WRITE_REG(GPIO_reg[GPIOx_en]->MODER, 3UL, (pos_u8 * 2U), 1UL);
				break;

			case AF:
				WRITE_REG(GPIO_reg[GPIOx_en]->MODER, 3UL, (pos_u8 * 2U), 2UL);

				if (7 >= pos_u8)
				{
					WRITE_REG(GPIO_reg[GPIOx_en]->AFRL, 0xFUL, (pos_u8 * 4U), AFx_en);
				}
				else
				{
					WRITE_REG(GPIO_reg[GPIOx_en]->AFRH, 0xFUL, ((pos_u8 - 8U) * 4U), AFx_en);
				}

				break;

			case Analog:
				WRITE_REG(GPIO_reg[GPIOx_en]->MODER, 3UL, (pos_u8 * 2U), 3UL);
				break;

			default:
				returnVal = NOT_OK;
				break;
		}

		switch (GPIO_outputType_en)
		{
			case PP:
				CLEAR_BIT(GPIO_reg[GPIOx_en]->OTYPER, pos_u8);
				break;

			case OD:
				SET_BIT(GPIO_reg[GPIOx_en]->OTYPER, pos_u8);
				break;

			case None:
				break;

			default:
				returnVal = NOT_OK;
				break;
		}

		// Set Output speed: High speed
		WRITE_REG(GPIO_reg[GPIOx_en]->OSPEEDR, 3UL, (pos_u8 * 2U), 2UL);

		switch (GPIO_pullDir_en)
		{
			case PU:
				WRITE_REG(GPIO_reg[GPIOx_en]->PUPDR, 3UL, (pos_u8 * 2), 1UL);
				break;

			case PD:
				WRITE_REG(GPIO_reg[GPIOx_en]->PUPDR, 3UL, (pos_u8 * 2), 2UL);
				break;

			case NoP:
				WRITE_REG(GPIO_reg[GPIOx_en]->PUPDR, 3UL, (pos_u8 * 2), 0UL);
				break;

			default:
				returnVal = NOT_OK;
				break;
		}
	}
	
	return returnVal;
}

int GPIO_IN_getVal(GPIO_ENABLE_t GPIOx_en, uint8_t pos_u8)
{
	bool GPIO_IN_val_b = 0;

	if ((15U < pos_u8) || (GPIOAEN > GPIOx_en) || (GPIOIEN < GPIOx_en))
	{
		return FAIL;
	}
	else
	{
		GPIO_IN_val_b = READ_REG(GPIO_reg[GPIOx_en]->IDR, 1UL, pos_u8);
	}

	return GPIO_IN_val_b;
}

bool GPIO_OUT_setVal(GPIO_ENABLE_t GPIOx_en, uint8_t pos_u8, bool setVal_b)
{
	bool returnVal = OK;

	if ((15U < pos_u8) || (GPIOAEN > GPIOx_en) || (GPIOIEN < GPIOx_en))
	{
		returnVal = NOT_OK;
		return returnVal;
	}
	else
	{
		if (1 == setVal_b)
		{
			SET_BIT(GPIO_reg[GPIOx_en]->ODR, pos_u8);
		}
		else
		{
			CLEAR_BIT(GPIO_reg[GPIOx_en]->ODR, pos_u8);
		}	
	}

	return returnVal;
}

static void GPIO_init(GPIO_ENABLE_t GPIOx_en)
{
	SET_BIT(RCC_reg->AHB1ENR, GPIOx_en);
}
