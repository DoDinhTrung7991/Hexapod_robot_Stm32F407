#include "Timer.h"

/*Variable - Start*/

static bool is_PWMinit[MAX_TIMER_NUMBER][MAX_CHANNEL_NUMBER] = { 0 };
const unsigned int OCxM_bit[2] = {4U, 12U};
const unsigned int OCxPE_bit[2] = {3U, 11U};
const unsigned int OCxFE_bit[2] = {2U, 10U};

GPIO_TimerX_t GPIO_TimerX_channel[14][4] =
{
	{
		{GPIOAEN, 8U, AF1},
		{GPIOAEN, 9U, AF1},
		{GPIOAEN, 10U, AF1},
		{GPIOAEN, 11U, AF1}
	},
	{
		{GPIOAEN, 0U, AF1},
		{GPIOAEN, 1U, AF1},
		{GPIOAEN, 2U, AF1},
		{GPIOAEN, 3U, AF1}
	},
	{
		{GPIOAEN, 6U, AF2},
		{GPIOAEN, 7U, AF2},
		{GPIOBEN, 0U, AF2},
		{GPIOBEN, 1U, AF2}
	},
	{
		{GPIOBEN, 6U, AF2},
		{GPIOBEN, 7U, AF2},
		{GPIOBEN, 8U, AF2},
		{GPIOBEN, 9U, AF2}
	},
	{
		{GPIOAEN, 0U, AF2},
		{GPIOAEN, 1U, AF2},
		{GPIOAEN, 2U, AF2},
		{GPIOAEN, 3U, AF2}
	},
	{
		{GPIOCEN, 6U, AF3},
		{GPIOCEN, 7U, AF3},
		{GPIOCEN, 8U, AF3},
		{GPIOCEN, 9U, AF3}
	},
	{
		{GPIOEEN, 5U, AF3},
		{GPIOEEN, 6U, AF3},
		{0, 0, 0},
		{0, 0, 0}
	},
	{
		{GPIOBEN, 8U, AF3},
		{0, 0, 0},
		{0, 0, 0},
		{0, 0, 0}
	},
	{
		{GPIOBEN, 9U, AF3},
		{0, 0, 0},
		{0, 0, 0},
		{0, 0, 0}
	},
	{
		{GPIOBEN, 14U, AF9},
		{GPIOBEN, 15U, AF9},
		{0, 0, 0},
		{0, 0, 0}
	},
	{
		{GPIOAEN, 6U, AF9},
		{0, 0, 0},
		{0, 0, 0},
		{0, 0, 0}
	},
	{
		{GPIOAEN, 7U, AF9},
		{0, 0, 0},
		{0, 0, 0},
		{0, 0, 0}
	}
};

/*Variable - End*/

bool PWM_init(TIMx_t TIMx_en, TIM_Channel_t Channel, uint8_t frequency_u8)
{
	is_PWMinit[TIMx_en][Channel] = false;

	if (OK != (Timer_init(TIMx_en, frequency_u8, false))
		||	(MAX_TIMER_NUMBER < TIMx_en)
		|| 	(TIM6 == TIMx_en)
		|| 	(TIM7 == TIMx_en)
		|| 	(
				((TIM9 == TIMx_en) || (TIM12 == TIMx_en))
				&& (CHANN_2 < Channel)
			)
		|| 	(
				(
					(TIM10 == TIMx_en)
					|| (TIM11 == TIMx_en)
					|| (TIM13 == TIMx_en)
					|| (TIM14 == TIMx_en)
				)
				&& (CHANN_1 < Channel)
			)
	)
	{
		return NOT_OK;
	}

	if ((TIM1 == TIMx_en) || (TIM8 == TIMx_en))
	{
		// Main output enable
		SET_BIT(TIM_reg[TIMx_en]->BDTR, 15U);
	}
	else
	{
		// do nothing
	}

	// Disable Counter
	CLEAR_BIT(TIM_reg[TIMx_en]->CR1, 0U);
	// CCx channel is configured as output
	WRITE_REG(TIM_reg[TIMx_en]->CCMR[Channel / 2U], 3UL, ((Channel % 2) * 8), 0UL);
	// Choose Output compare mode
	WRITE_REG(TIM_reg[TIMx_en]->CCMR[Channel / 2U], 7UL, OCxM_bit[Channel % 2], 6UL);
	// Output compare preload enable
	SET_BIT(TIM_reg[TIMx_en]->CCMR[Channel / 2U], OCxPE_bit[Channel % 2]);
	// Output compare fast enable
	SET_BIT(TIM_reg[TIMx_en]->CCMR[Channel / 2U], OCxFE_bit[Channel % 2]);
	// Output compare enable
	SET_BIT(TIM_reg[TIMx_en]->CCER, (Channel * 4U));
	// Setup GPIO
	GPIO_setup(
		GPIO_TimerX_channel[TIMx_en][Channel].GPIOx_en,
		GPIO_TimerX_channel[TIMx_en][Channel].pos_u8,
		AF,
		GPIO_TimerX_channel[TIMx_en][Channel].AFx_en,
		PP,
		NoP
	);
	is_PWMinit[TIMx_en][Channel] = true;

	return OK;
}

bool PWM_Generation(TIMx_t TIMx_en, TIM_Channel_t Channel, float activePercent)
{
	if ((true == is_timerInit[TIMx_en]) && (true == is_PWMinit[TIMx_en][Channel]))
	{
		TIM_reg[TIMx_en]->CCR[Channel] = (unsigned short)(((ARR_CONST + 1) * activePercent) / 100U);
	}
	else
	{
		return NOT_OK;
	}

	// Reset Counter
	TIM_reg[TIMx_en]->CNT = 0U;
	// Enable Counter
	SET_BIT(TIM_reg[TIMx_en]->CR1, 0U);

	return OK;
}
