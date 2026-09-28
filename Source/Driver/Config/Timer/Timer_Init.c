#include "Timer.h"
#include "init_static.h"
#include "RCC_header.h"
#include "Interrupt.h"

TIM_t * const TIM_reg[14] = 
{
	(TIM_t*)0x40010000,
	(TIM_t*)0x40000000,
	(TIM_t*)0x40000400,
	(TIM_t*)0x40000800,
	(TIM_t*)0x40000C00,
	(TIM_t*)0x40001000,
	(TIM_t*)0x40001400,
	(TIM_t*)0x40010400,
	(TIM_t*)0x40014000,
	(TIM_t*)0x40014400,
	(TIM_t*)0x40014800,
	(TIM_t*)0x40001800,
	(TIM_t*)0x40001C00,
	(TIM_t*)0x40002000
};

static RCC_APBxENR_enable_t RCC_APBxENR_TimerX_arr[14] =
{
	{&RCC_reg->APB2ENR, 0U},
	{&RCC_reg->APB1ENR, 0U},
	{&RCC_reg->APB1ENR, 1U},
	{&RCC_reg->APB1ENR, 2U},
	{&RCC_reg->APB1ENR, 3U},
	{&RCC_reg->APB1ENR, 4U},
	{&RCC_reg->APB1ENR, 5U},
	{&RCC_reg->APB2ENR, 1U},
	{&RCC_reg->APB2ENR, 16U},
	{&RCC_reg->APB2ENR, 17U},
	{&RCC_reg->APB2ENR, 18U},
	{&RCC_reg->APB1ENR, 6U},
	{&RCC_reg->APB1ENR, 7U},
	{&RCC_reg->APB1ENR, 8U}
};

static const peripheral_Selection_t TimerX_Interrupt_line[14] =
{
	TIM1_UP_TIM10,
	TIM2_Interrupt,
	TIM3_Interrupt,
	TIM4_Interrupt,
	TIM5_Interrupt,
	TIM6_DAC,
	TIM7_Interrupt,
	TIM8_UP_TIM13,
	TIM1_BRK_TIM9,
	TIM1_UP_TIM10,
	TIM1_TRG_COM_TIM11,
	TIM8_BRK_TIM12,
	TIM8_UP_TIM13,
	TIM8_TRG_COM_TIM14
};

uint8_t TIMxFrequency[MAX_TIMER_NUMBER] = { 0 };
bool is_timerInit[MAX_TIMER_NUMBER] = { 0 };

bool Timer_init(TIMx_t TIMx_en, uint8_t frequency_u8, bool is_Interrupt_b)
{
	unsigned int APBx_freq_TimerX_arr[14] =
	{
		APB2_freq,
		APB1_freq,
		APB1_freq,
		APB1_freq,
		APB1_freq,
		APB1_freq,
		APB1_freq,
		APB2_freq,
		APB2_freq,
		APB2_freq,
		APB2_freq,
		APB1_freq,
		APB1_freq,
		APB1_freq
	};
	
	is_timerInit[TIMx_en] = false;
	TIMxFrequency[TIMx_en] = frequency_u8;

	// Enable Timer peripheral and interrupt line
	SET_BIT(*RCC_APBxENR_TimerX_arr[TIMx_en].reg_u32_ptr, RCC_APBxENR_TimerX_arr[TIMx_en].pos);
	NVIC_ISER_setVal(TimerX_Interrupt_line[TIMx_en]);

	// Disable Counter
	CLEAR_BIT(TIM_reg[TIMx_en]->CR1, 0U);
	// Update generation
	SET_BIT(TIM_reg[TIMx_en]->EGR, 0U);

	TIM_reg[TIMx_en]->ARR = (uint16_t)ARR_CONST;
	TIM_reg[TIMx_en]->PSC = (uint16_t)(APBx_freq_TimerX_arr[TIMx_en] / ((ARR_CONST + 1U) * TIMxFrequency[TIMx_en]) - 1);

	// Auto-reload preload enable
	SET_BIT(TIM_reg[TIMx_en]->CR1, 7U);
	// Clear interrupt flag
	CLEAR_BIT(TIM_reg[TIMx_en]->SR, 0U);

	if (is_Interrupt_b)
	{
		// Update interrupt enable
		SET_BIT(TIM_reg[TIMx_en]->DIER, 0U);
	}
	
	// Enable Counter
	SET_BIT(TIM_reg[TIMx_en]->CR1, 0U);
	is_timerInit[TIMx_en] = true;

	return OK;
}
