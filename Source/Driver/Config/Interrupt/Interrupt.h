#ifndef _H_INTERRUPT_
#define _H_INTERRUPT_

#include "stdUtility.h"
#include "GPIO.h"

typedef enum
{
    WWDG,
    PVD,
    TAMP_STAMP,
    RTC_WKUP,
    FLASH,
    RCC,
    EXTI0,
    EXTI1,
    EXTI2,
    EXTI3,
    EXTI4,
    DMA1_Stream0,
    DMA1_Stream1,
    DMA1_Stream2,
    DMA1_Stream3,
    DMA1_Stream4,
    DMA1_Stream5,
    DMA1_Stream6,
    ADC,
    CAN1_TX,
    CAN1_RX0,
    CAN1_RX1,
    CAN1_SCE,
    EXTI9_5,
    TIM1_BRK_TIM9,
    TIM1_UP_TIM10,
    TIM1_TRG_COM_TIM11,
    TIM1_CC,
    TIM2_Interrupt,
    TIM3_Interrupt,
    TIM4_Interrupt,
    I2C1_EV,
    I2C1_ER,
    I2C2_EV,
    I2C2_ER,
    SPI1_Interrupt,
    SPI2_Interrupt,
    USART1_Interrupt,
    USART2_Interrupt,
    USART3_Interrupt,
    EXTI15_10,
    RTC_Alarm,
    OTG_FS_WKUP,
    TIM8_BRK_TIM12,
    TIM8_UP_TIM13,
    TIM8_TRG_COM_TIM14,
    TIM8_CC,
    DMA1_Stream7,
    FSMC,
    SDIO,
    TIM5_Interrupt,
    SPI3_Interrupt,
    UART4_Interrupt,
    UART5_Interrupt,
    TIM6_DAC,
    TIM7_Interrupt,
    DMA2_Stream0,
    DMA2_Stream1,
    DMA2_Stream2,
    DMA2_Stream3,
    DMA2_Stream4,
    ETH,
    ETH_WKUP,
    CAN2_TX,
    CAN2_RX0,
    CAN2_RX1,
    CAN2_SCE,
    OTG_FS,
    DMA2_Stream5,
    DMA2_Stream6,
    DMA2_Stream7,
    USART6_Interrupt,
    I2C3_EV,
    I2C3_ER
} peripheral_Selection_t;

typedef enum
{
    Falling_Edge,
    Rising_Edge,
    Both_Edge
} EvRaising_Dir_t;

extern volatile uint32_t SysTick_cnt_u32;

void NVIC_ISER_setVal(peripheral_Selection_t peripheral_Selection_en);
void NVIC_ICER_setVal(peripheral_Selection_t peripheral_Selection_en);
bool Ex_Interrupt(GPIO_ENABLE_t GPIOx_en, uint8_t pos_u8, EvRaising_Dir_t EvRaising_Dir_en);

#endif
