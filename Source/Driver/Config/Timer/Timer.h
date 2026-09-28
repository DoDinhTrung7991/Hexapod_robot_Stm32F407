#ifndef _TIMER_H_
#define _TIMER_H_

#include "stdUtility.h"
#include "bit_operator.h"
#include "TIM_header.h"
#include "GPIO.h"

/*Macro - Start*/

#define MAX_TIMER_NUMBER 14U
#define MAX_CHANNEL_NUMBER 4U
#define ARR_CONST 99UL

/*Macro - End*/

/*Data type - Start*/

typedef enum
{
	TIM1,
	TIM2,
	TIM3,
	TIM4,
	TIM5,
	TIM6,
	TIM7,
	TIM8,
	TIM9,
	TIM10,
	TIM11,
	TIM12,
	TIM13,
	TIM14
} TIMx_t;

typedef enum
{
	CHANN_1,
	CHANN_2,
	CHANN_3,
	CHANN_4
} TIM_Channel_t;

typedef struct
{
    GPIO_ENABLE_t GPIOx_en;
    uint8_t pos_u8;
	AFRx_t AFx_en;
} GPIO_TimerX_t;

extern GPIO_TimerX_t GPIO_TimerX_channel[14][4];

/*Data type - End*/

/*Variable - Start*/

extern bool is_timerInit[MAX_TIMER_NUMBER];
extern uint8_t TIMxFrequency[MAX_TIMER_NUMBER];

/*Variable - End*/

/*Function - Start*/

// Init Timer basic
bool Timer_init(TIMx_t TIMx_en, uint8_t frequency_u8, bool is_Interrupt_b);

// PWM
bool PWM_init(TIMx_t TIMx_en, TIM_Channel_t Channel, uint8_t frequency_u8);
bool PWM_Generation(TIMx_t TIMx_en, TIM_Channel_t Channel, float activePercent);

/*Function - End*/

#endif
