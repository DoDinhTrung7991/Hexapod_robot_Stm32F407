#include "stdUtility.h"
#include "bit_operator.h"
#include "main.h"
#include "init.h"
#include "GPIO.h"
#include "EXTI_header.h"
#include "DMA_header.h"
#include "UART.h"
#include "I2C.h"
#include "Timer.h"
#include "queue.h"

typedef void (*isr_fnct_t)(void);

extern uint32_t _ccmram_sidata;
extern uint32_t _ccmram_sdata;
extern uint32_t _ccmram_edata;
extern uint32_t _ccmram_sbss;
extern uint32_t _ccmram_ebss;
extern uint32_t _sidata;
extern uint32_t _sdata;
extern uint32_t _edata;
extern uint32_t _sbss;
extern uint32_t _ebss;
extern uint32_t _estack;

__attribute__((section(".ccmram_bss")))volatile uint32_t SysTick_cnt_u32;

void Default_Handler(void);

void Reset_Handler(void);
__attribute__((weak, alias("Default_Handler")))void NMI_Handler(void);
__attribute__((weak, alias("Default_Handler")))void HardFault_Handler(void);
__attribute__((weak, alias("Default_Handler")))void MemManage_Handler(void);
__attribute__((weak, alias("Default_Handler")))void BusFault_Handler(void);
__attribute__((weak, alias("Default_Handler")))void UsageFault_Handler(void);
__attribute__((weak, alias("Default_Handler")))void SVCall_Handler(void);
__attribute__((weak, alias("Default_Handler")))void Debug_Handler(void);
__attribute__((weak, alias("Default_Handler")))void PendSV_Handler(void);
void SysTick_Handler(void);
__attribute__((weak, alias("Default_Handler")))void WWDG_Handler(void);
__attribute__((weak, alias("Default_Handler")))void PVD_Handler(void); 
__attribute__((weak, alias("Default_Handler")))void TAMP_STAMP_Handler(void);
__attribute__((weak, alias("Default_Handler")))void RTC_WKUP_Handler(void);
__attribute__((weak, alias("Default_Handler")))void FLASH_Handler(void);
__attribute__((weak, alias("Default_Handler")))void RCC_Handler(void);
void EXTI0_Handler(void);
__attribute__((weak, alias("Default_Handler")))void EXTI1_Handler(void);
__attribute__((weak, alias("Default_Handler")))void EXTI2_Handler(void);
void EXTI3_Handler(void);
void EXTI4_Handler(void);
void DMA1_Stream0_Handler(void);
void DMA1_Stream1_Handler(void);
void DMA1_Stream2_Handler(void);
void DMA1_Stream3_Handler(void);
void DMA1_Stream4_Handler(void);
void DMA1_Stream5_Handler(void);
void DMA1_Stream6_Handler(void);
__attribute__((weak, alias("Default_Handler")))void ADC_Handler(void);
__attribute__((weak, alias("Default_Handler")))void CAN1_TX_Handler(void);
__attribute__((weak, alias("Default_Handler")))void CAN1_RX0_Handler(void);
__attribute__((weak, alias("Default_Handler")))void CAN1_RX1_Handler(void);
__attribute__((weak, alias("Default_Handler")))void CAN1_SCE_Handler(void);
__attribute__((weak, alias("Default_Handler")))void EXTI9_5_Handler(void);
__attribute__((weak, alias("Default_Handler")))void TIM1_BRK_TIM9_Handler(void);
__attribute__((weak, alias("Default_Handler")))void TIM1_UP_TIM10_Handler(void);
__attribute__((weak, alias("Default_Handler")))void TIM1_TRG_COM_TIM11_Handler(void);
__attribute__((weak, alias("Default_Handler")))void TIM1_CC_Handler(void);
void TIM2_Handler(void);
__attribute__((weak, alias("Default_Handler")))void TIM3_Handler(void);
__attribute__((weak, alias("Default_Handler")))void TIM4_Handler(void);
void I2C1_EV_Handler(void);
void I2C1_ER_Handler(void);
__attribute__((weak, alias("Default_Handler")))void I2C2_EV_Handler(void);
__attribute__((weak, alias("Default_Handler")))void I2C2_ER_Handler(void);
__attribute__((weak, alias("Default_Handler")))void SPI1_Handler(void);
__attribute__((weak, alias("Default_Handler")))void SPI2_Handler(void);
void USART1_Handler(void);
__attribute__((weak, alias("Default_Handler")))void USART2_Handler(void);
__attribute__((weak, alias("Default_Handler")))void USART3_Handler(void);
void EXTI15_10_Handler(void);
__attribute__((weak, alias("Default_Handler")))void RTC_Alarm_Handler(void);
__attribute__((weak, alias("Default_Handler")))void OTG_FS_WKUP_Handler(void);
__attribute__((weak, alias("Default_Handler")))void TIM8_BRK_TIM12_Handler(void);
void TIM8_UP_TIM13_Handler(void);
__attribute__((weak, alias("Default_Handler")))void TIM8_TRG_COM_TIM14_Handler(void);
__attribute__((weak, alias("Default_Handler")))void TIM8_CC_Handler(void);
void DMA1_Stream7_Handler(void);
__attribute__((weak, alias("Default_Handler")))void FSMC_Handler(void);
__attribute__((weak, alias("Default_Handler")))void SDIO_Handler(void);
__attribute__((weak, alias("Default_Handler")))void TIM5_Handler(void);
__attribute__((weak, alias("Default_Handler")))void SPI3_Handler(void);
__attribute__((weak, alias("Default_Handler")))void UART4_Handler(void);
__attribute__((weak, alias("Default_Handler")))void UART5_Handler(void);
__attribute__((weak, alias("Default_Handler")))void TIM6_DAC_Handler(void);
__attribute__((weak, alias("Default_Handler")))void TIM7_Handler(void);
void DMA2_Stream0_Handler(void);
void DMA2_Stream1_Handler(void);
void DMA2_Stream2_Handler(void);
void DMA2_Stream3_Handler(void);
void DMA2_Stream4_Handler(void);
__attribute__((weak, alias("Default_Handler")))void ETH_Handler(void);
__attribute__((weak, alias("Default_Handler")))void ETH_WKUP_Handler(void);
__attribute__((weak, alias("Default_Handler")))void CAN2_TX_Handler(void);
__attribute__((weak, alias("Default_Handler")))void CAN2_RX0_Handler(void);
__attribute__((weak, alias("Default_Handler")))void CAN2_RX1_Handler(void);
__attribute__((weak, alias("Default_Handler")))void CAN2_SCE_Handler(void);
__attribute__((weak, alias("Default_Handler")))void OTG_FS_Handler(void);
void DMA2_Stream5_Handler(void);
void DMA2_Stream6_Handler(void);
void DMA2_Stream7_Handler(void);
__attribute__((weak, alias("Default_Handler")))void USART6_Handler(void);
__attribute__((weak, alias("Default_Handler")))void I2C3_EV_Handler(void);
__attribute__((weak, alias("Default_Handler")))void I2C3_ER_Handler(void);
__attribute__((weak, alias("Default_Handler")))void OTG_HS_EP1_OUT_Handler(void);
__attribute__((weak, alias("Default_Handler")))void OTG_HS_EP1_IN_Handler(void);
__attribute__((weak, alias("Default_Handler")))void OTG_HS_WKUP_Handler(void);
__attribute__((weak, alias("Default_Handler")))void OTG_HS_Handler(void);
__attribute__((weak, alias("Default_Handler")))void DCMI_Handler(void);
__attribute__((weak, alias("Default_Handler")))void CRYP_Handler(void);
__attribute__((weak, alias("Default_Handler")))void HASH_RNG_Handler(void);
__attribute__((weak, alias("Default_Handler")))void FPU_Handler(void);

__attribute__((section(".isr_vector"), used)) isr_fnct_t isr_vector_arr[] = // vector table placed at .isr_vector section
{
    (isr_fnct_t)&_estack,
    Reset_Handler,
    NMI_Handler,
    HardFault_Handler,
    MemManage_Handler,
    BusFault_Handler,
    UsageFault_Handler,
    (isr_fnct_t)0,
    (isr_fnct_t)0,
    (isr_fnct_t)0,
    (isr_fnct_t)0,
    SVCall_Handler,
    Debug_Handler,
    (isr_fnct_t)0,
    PendSV_Handler,
    SysTick_Handler,
    WWDG_Handler,
    PVD_Handler,
    TAMP_STAMP_Handler,
    RTC_WKUP_Handler,
    FLASH_Handler,
    RCC_Handler,
    EXTI0_Handler,
    EXTI1_Handler,
    EXTI2_Handler,
    EXTI3_Handler,
    EXTI4_Handler,
    DMA1_Stream0_Handler,
    DMA1_Stream1_Handler,
    DMA1_Stream2_Handler,
    DMA1_Stream3_Handler,
    DMA1_Stream4_Handler,
    DMA1_Stream5_Handler,
    DMA1_Stream6_Handler,
    ADC_Handler,
    CAN1_TX_Handler,
    CAN1_RX0_Handler,
    CAN1_RX1_Handler,
    CAN1_SCE_Handler,
    EXTI9_5_Handler,
    TIM1_BRK_TIM9_Handler,
    TIM1_UP_TIM10_Handler,
    TIM1_TRG_COM_TIM11_Handler,
    TIM1_CC_Handler,
    TIM2_Handler,
    TIM3_Handler,
    TIM4_Handler,
    I2C1_EV_Handler,
    I2C1_ER_Handler,
    I2C2_EV_Handler,
    I2C2_ER_Handler,
    SPI1_Handler,
    SPI2_Handler,
    USART1_Handler,
    USART2_Handler,
    USART3_Handler,
    EXTI15_10_Handler,
    RTC_Alarm_Handler,
    OTG_FS_WKUP_Handler,
    TIM8_BRK_TIM12_Handler,
    TIM8_UP_TIM13_Handler,
    TIM8_TRG_COM_TIM14_Handler,
    TIM8_CC_Handler,
    DMA1_Stream7_Handler,
    FSMC_Handler,
    SDIO_Handler,
    TIM5_Handler,
    SPI3_Handler,
    UART4_Handler,
    UART5_Handler,
    TIM6_DAC_Handler,
    TIM7_Handler,
    DMA2_Stream0_Handler,
    DMA2_Stream1_Handler,
    DMA2_Stream2_Handler,
    DMA2_Stream3_Handler,
    DMA2_Stream4_Handler,
    ETH_Handler,
    ETH_WKUP_Handler,
    CAN2_TX_Handler,
    CAN2_RX0_Handler,
    CAN2_RX1_Handler,
    CAN2_SCE_Handler,
    OTG_FS_Handler,
    DMA2_Stream5_Handler,
    DMA2_Stream6_Handler,
    DMA2_Stream7_Handler,
    USART6_Handler,
    I2C3_EV_Handler,
    I2C3_ER_Handler,
    OTG_HS_EP1_OUT_Handler,
    OTG_HS_EP1_IN_Handler,
    OTG_HS_WKUP_Handler,
    OTG_HS_Handler,
    DCMI_Handler,
    CRYP_Handler,
    HASH_RNG_Handler,
    FPU_Handler,
};

static inline void LMAtoVMA(uint32_t *pDst, uint32_t *pSrc, uint32_t *pDst_end);
static inline void clear_bss(uint32_t *pStart, uint32_t *pEnd);

void Reset_Handler(void)
{
    // Copy LMA data to VMA
    LMAtoVMA(&_sdata, &_sidata, &_edata);
    LMAtoVMA(&_ccmram_sdata, &_ccmram_sidata, &_ccmram_edata);

    // Clear bss memory
    clear_bss(&_sbss, &_ebss);
    clear_bss(&_ccmram_sbss, &_ccmram_ebss);

    if (OK == SystemInit()) // Check if System init being successful
    {
        (void)main();
    }
    else
    {
        // do nothing
    }

    while (1)
    {
        // do nothing
    }
}

void EXTI0_Handler(void)
{
    if (READ_REG(EXTI_reg->PR, 1UL, 0U))
	{
        // Clear the pending bit by writing 1 to it
        SET_BIT(EXTI_reg->PR, 0U);
	}
}

void EXTI3_Handler(void)
{
    if (0 == GPIO_IN_getVal(GPIOEEN, 3))
    {
        GPIO_OUT_setVal(GPIOAEN, 6, 0);
    }

	if (READ_REG(EXTI_reg->PR, 1UL, 3U))
	{
        // Clear the pending bit by writing 1 to it
        SET_BIT(EXTI_reg->PR, 3U);
	}
}

void EXTI4_Handler(void)
{
    if (0 == GPIO_IN_getVal(GPIOEEN, 4))
    {
        GPIO_OUT_setVal(GPIOAEN, 6, 1);
    }

	if (READ_REG(EXTI_reg->PR, 1UL, 4U))
	{
		// Clear the pending bit by writing 1 to it
        SET_BIT(EXTI_reg->PR, 4U);
	}
}

void EXTI15_10_Handler(void)
{
    if ((INITTED == UART_init_state[USART1]) && (UART_STATE_READY == UART_state_rx[USART1]))
    {
        UART_state_rx[USART1] = UART_STATE_BUSY;

        //Disable EXTI
        CLEAR_BIT(EXTI_reg->IMR, 10U);
    }
    
    for (uint8_t i = 10; i <= 15; i ++)
    {
        if (READ_REG(EXTI_reg->PR, 1UL, i))
	    {
	    	// Clear the pending bit by writing 1 to it
            SET_BIT(EXTI_reg->PR, i);
	    }
    }
}

void USART1_Handler(void)
{
    // Check IDLE flag
    if (READ_REG(USART_reg[USART1]->SR, 1UL, 4U))
    {
        UART_recv_buf[USART1].isEmpty = false;
        UART_recv_buf[USART1].rear = (uint8_t)(sizeof(UART_recv_buf[USART1].buf) - DMA_reg[DMA2]->S[2].NDTR - 1);

        if (true == UART_recv_buf[USART1].isFull)
        {
            UART_recv_buf[USART1].front = (UART_recv_buf[USART1].rear + 1) % sizeof(UART_recv_buf[USART1].buf);
        }

        if (
            (UART_recv_buf[USART1].front <= ((UART_recv_buf[USART1].rear + 1) % sizeof(UART_recv_buf[USART1].buf)))
            && (false == UART_recv_buf[USART1].isEmpty)
            && (UART_recv_buf[USART1].overrun)
        )
        {
            UART_recv_buf[USART1].isFull = true;
        }

        isUpdated_UART[USART1] = true;

        // Clear IDLE flag
        uint32_t temp = USART_reg[USART1]->SR;
        temp = USART_reg[USART1]->DR;
        (void)temp;
        UART_state_rx[USART1] = UART_STATE_READY;

        // Clear the pending bit by writing 1 to it
        SET_BIT(EXTI_reg->PR, 10);
        //Enable EXTI
        SET_BIT(EXTI_reg->IMR, 10U);
    }

    // Check Overrun error, Noise ,Framing error and Parity error
    if (READ_REG(USART_reg[USART1]->SR, 1UL, 3U) || READ_REG(USART_reg[USART1]->SR, 1UL, 2U) || READ_REG(USART_reg[USART1]->SR, 1UL, 1U) || READ_REG(USART_reg[USART1]->SR, 1UL, 0U))
    {
        // Clear error flags
        uint32_t temp = USART_reg[USART1]->SR;
        temp = USART_reg[USART1]->DR;
        (void)temp;

        if (UART_STATE_BUSY == UART_state_rx[USART1])
        {
            UART_state_rx[USART1] = UART_STATE_READY;

            // Clear the pending bit by writing 1 to it
            SET_BIT(EXTI_reg->PR, 10);
            //Enable EXTI
            SET_BIT(EXTI_reg->IMR, 10U);
        }
    }
}

void I2C1_EV_Handler(void)
{
    static bool isLastByte_I2C[2];

    // Check Start bit
    if (READ_REG(I2C_reg[I2C1]->SR1, 1UL, 0U))
    {
        // Write Slave address
        WRITE_REG(I2C_reg[I2C1]->DR, 0xFFUL, 0U, I2C_addr[I2C1]);
        isLastByte_I2C[0] = true;
        isLastByte_I2C[1] = false;
    }

    // Check Address sent
    if (READ_REG(I2C_reg[I2C1]->SR1, 1UL, 1U))
    {
        // Disable Acknowledge if only write/read 1 byte
        if (
            ((1U == READ_REG(DMA_reg[DMA1]->S[5].NDTR, 0xFFFFU, 0U)) && (I2C_STATE_BUSY == I2C_state_rx[I2C1]))
            || ((1U == READ_REG(DMA_reg[DMA1]->S[7].NDTR, 0xFFFFU, 0U)) && (I2C_STATE_BUSY == I2C_state_tx[I2C1]))
        )
        {
            // Clear ACK
            CLEAR_BIT(I2C_reg[I2C1]->CR1, 10U);
        }

        // Check TRA bit in the SR2 value we just read
        if (
            ((SEND == READ_REG(I2C_reg[I2C1]->SR2, 1UL, 2U)) && (I2C_STATE_READY == I2C_state_tx[I2C1]))
            || ((RECV == READ_REG(I2C_reg[I2C1]->SR2, 1UL, 2U)) && (I2C_STATE_READY == I2C_state_rx[I2C1]))
        )
        {
            // Reset states to READY so the system can try again
            I2C_state_tx[I2C1] = I2C_STATE_READY;
            I2C_state_rx[I2C1] = I2C_STATE_READY;
        }
    }

    // Check RxNE
    if (READ_REG(I2C_reg[I2C1]->SR1, 1UL, 6U))
    {
        if ((true == isLastByte_I2C[0]) && (false == isLastByte_I2C[1]))
        {
            I2C_recv_buf[I2C1][I2C_length[I2C1] - 2] = READ_REG(I2C_reg[I2C1]->DR, 0xFFU, 0U);
            isLastByte_I2C[0] = false;
            isLastByte_I2C[1] = true;
            // Clear ACK
            CLEAR_BIT(I2C_reg[I2C1]->CR1, 10U);
            // Send Stop condition
            SET_BIT(I2C_reg[I2C1]->CR1, 9U);
        }

        if ((false == isLastByte_I2C[0]) && (true == isLastByte_I2C[1]))
        {
            I2C_recv_buf[I2C1][I2C_length[I2C1] - 1] = READ_REG(I2C_reg[I2C1]->DR, 0xFFU, 0U);
            I2C_state_rx[I2C1] = I2C_STATE_READY;
            isUpdated_I2C[I2C1] = true;
        }
    }

    // Check BTF
    if (READ_REG(I2C_reg[I2C1]->SR1, 1UL, 2U))
    {
        if (I2C_STATE_BUSY == I2C_state_tx[I2C1])
        {
            // Send Stop condition
            SET_BIT(I2C_reg[I2C1]->CR1, 9U);
            I2C_state_tx[I2C1] = I2C_STATE_READY;
        }
    }
}

void I2C1_ER_Handler(void)
{
    // Check for Bus error (BERR)
    if (READ_REG(I2C_reg[I2C1]->SR1, 1UL, 8U))
    {
        // Clear BERR flag
        CLEAR_BIT(I2C_reg[I2C1]->SR1, 8U);
        // Reset states to READY so the system can try again
        I2C_state_tx[I2C1] = I2C_STATE_READY;
        I2C_state_rx[I2C1] = I2C_STATE_READY;
    }

    // Check for Arbitration lost (ARLO)
    if (READ_REG(I2C_reg[I2C1]->SR1, 1UL, 9U))
    {
        // Clear ARLO flag
        CLEAR_BIT(I2C_reg[I2C1]->SR1, 9U);
        // Reset states to READY so the system can try again
        I2C_state_tx[I2C1] = I2C_STATE_READY;
        I2C_state_rx[I2C1] = I2C_STATE_READY;
    }

    // Check for Acknowledge Failure (AF)
    if (READ_REG(I2C_reg[I2C1]->SR1, 1UL, 10U))
    {
        // Check bit MSL: Master/slave
        if (READ_REG(I2C_reg[I2C1]->SR2, 1UL, 0U))
        {
            // Send Stop condition
            SET_BIT(I2C_reg[I2C1]->CR1, 9U);
        }

        // Clear AF flag
        CLEAR_BIT(I2C_reg[I2C1]->SR1, 10U);
        // Reset states to READY so the system can try again
        I2C_state_tx[I2C1] = I2C_STATE_READY;
        I2C_state_rx[I2C1] = I2C_STATE_READY;
    }

    // Check for Overrun/Underrun (OVR)
    if (READ_REG(I2C_reg[I2C1]->SR1, 1UL, 11U))
    {
        // Clear OVR flag
        CLEAR_BIT(I2C_reg[I2C1]->SR1, 11U);
    }

    // Check for Timeout or Tlow error (TIMEOUT)
    if (READ_REG(I2C_reg[I2C1]->SR1, 1UL, 14U))
    {
        // Check bit MSL: Master/slave
        if (READ_REG(I2C_reg[I2C1]->SR2, 1UL, 0U))
        {
            // Send Stop condition
            SET_BIT(I2C_reg[I2C1]->CR1, 9U);
        }
        
        // Clear TIMEOUT flag
        CLEAR_BIT(I2C_reg[I2C1]->SR1, 14U);
        // Reset states to READY so the system can try again
        I2C_state_tx[I2C1] = I2C_STATE_READY;
        I2C_state_rx[I2C1] = I2C_STATE_READY;
    }
}

void DMA1_Stream0_Handler(void)
{
    if (READ_REG(DMA_reg[DMA1]->ISR[0], 1UL, (interruptFlag_0or4 + TCIFx)))
    {
        // Clear TCIFx
        SET_BIT(DMA_reg[DMA1]->IFCR[0], (interruptFlag_0or4 + TCIFx));

        if (I2C_STATE_BUSY == I2C_state_rx[I2C1])
        {
            // Set Buffer interrupt enable
            SET_BIT(I2C_reg[I2C1]->CR2, 10U);
            // DMA requests disable
            CLEAR_BIT(I2C_reg[I2C1]->CR2, 11U);
        }
    }

    if (READ_REG(DMA_reg[DMA1]->ISR[0], 1UL, (interruptFlag_0or4 + DMEIFx)))
    {
        SET_BIT(DMA_reg[DMA1]->IFCR[0], (interruptFlag_0or4 + DMEIFx));
    }

    if (READ_REG(DMA_reg[DMA1]->ISR[0], 1UL, (interruptFlag_0or4 + TEIFx)))
    {
        SET_BIT(DMA_reg[DMA1]->IFCR[0], (interruptFlag_0or4 + TEIFx));
    }
}

void DMA1_Stream1_Handler(void)
{
    if (READ_REG(DMA_reg[DMA1]->ISR[0], 1UL, (interruptFlag_1or5 + TCIFx)))
    {
        // Clear TCIFx
        SET_BIT(DMA_reg[DMA1]->IFCR[0], (interruptFlag_1or5 + TCIFx));
    }

    if (READ_REG(DMA_reg[DMA1]->ISR[0], 1UL, (interruptFlag_1or5 + DMEIFx)))
    {
        SET_BIT(DMA_reg[DMA1]->IFCR[0], (interruptFlag_1or5 + DMEIFx));
    }

    if (READ_REG(DMA_reg[DMA1]->ISR[0], 1UL, (interruptFlag_1or5 + TEIFx)))
    {
        SET_BIT(DMA_reg[DMA1]->IFCR[0], (interruptFlag_1or5 + TEIFx));
    }
}

void DMA1_Stream2_Handler(void)
{
    if (READ_REG(DMA_reg[DMA1]->ISR[0], 1UL, (interruptFlag_2or6 + TCIFx)))
    {
        // Clear TCIFx
        SET_BIT(DMA_reg[DMA1]->IFCR[0], (interruptFlag_2or6 + TCIFx));

        if (I2C_STATE_BUSY == I2C_state_rx[I2C2])
        {
            // Set Buffer interrupt enable
            SET_BIT(I2C_reg[I2C2]->CR2, 10U);
            // DMA requests disable
            CLEAR_BIT(I2C_reg[I2C2]->CR2, 11U);
        }

        if (I2C_STATE_BUSY == I2C_state_rx[I2C3])
        {
            // Set Buffer interrupt enable
            SET_BIT(I2C_reg[I2C3]->CR2, 10U);
            // DMA requests disable
            CLEAR_BIT(I2C_reg[I2C3]->CR2, 11U);
        }
    }

    if (READ_REG(DMA_reg[DMA1]->ISR[0], 1UL, (interruptFlag_2or6 + DMEIFx)))
    {
        SET_BIT(DMA_reg[DMA1]->IFCR[0], (interruptFlag_2or6 + DMEIFx));
    }

    if (READ_REG(DMA_reg[DMA1]->ISR[0], 1UL, (interruptFlag_2or6 + TEIFx)))
    {
        SET_BIT(DMA_reg[DMA1]->IFCR[0], (interruptFlag_2or6 + TEIFx));
    }
}

void DMA1_Stream3_Handler(void)
{
    if (READ_REG(DMA_reg[DMA1]->ISR[0], 1UL, (interruptFlag_3or7 + TCIFx)))
    {
        // Clear TCIFx
        SET_BIT(DMA_reg[DMA1]->IFCR[0], (interruptFlag_3or7 + TCIFx));

        if (I2C_STATE_BUSY == I2C_state_rx[I2C2])
        {
            // Set Buffer interrupt enable
            SET_BIT(I2C_reg[I2C2]->CR2, 10U);
            // DMA requests disable
            CLEAR_BIT(I2C_reg[I2C2]->CR2, 11U);
        }
    }

    if (READ_REG(DMA_reg[DMA1]->ISR[0], 1UL, (interruptFlag_3or7 + DMEIFx)))
    {
        SET_BIT(DMA_reg[DMA1]->IFCR[0], (interruptFlag_3or7 + DMEIFx));
    }

    if (READ_REG(DMA_reg[DMA1]->ISR[0], 1UL, (interruptFlag_3or7 + TEIFx)))
    {
        SET_BIT(DMA_reg[DMA1]->IFCR[0], (interruptFlag_3or7 + TEIFx));
    }
}

void DMA1_Stream4_Handler(void)
{
    if (READ_REG(DMA_reg[DMA1]->ISR[1], 1UL, (interruptFlag_0or4 + TCIFx)))
    {
        // Clear TCIFx
        SET_BIT(DMA_reg[DMA1]->IFCR[1], (interruptFlag_0or4 + TCIFx));

        if (I2C_STATE_BUSY == I2C_state_tx[I2C3])
        {
            // DMA requests disable
            CLEAR_BIT(I2C_reg[I2C3]->CR2, 11U);
        }
    }

    if (READ_REG(DMA_reg[DMA1]->ISR[1], 1UL, (interruptFlag_0or4 + DMEIFx)))
    {
        SET_BIT(DMA_reg[DMA1]->IFCR[1], (interruptFlag_0or4 + DMEIFx));
    }

    if (READ_REG(DMA_reg[DMA1]->ISR[1], 1UL, (interruptFlag_0or4 + TEIFx)))
    {
        SET_BIT(DMA_reg[DMA1]->IFCR[1], (interruptFlag_0or4 + TEIFx));
    }
}

void DMA1_Stream5_Handler(void)
{
    if (READ_REG(DMA_reg[DMA1]->ISR[1], 1UL, (interruptFlag_1or5 + TCIFx)))
    {
        // Clear TCIFx
        SET_BIT(DMA_reg[DMA1]->IFCR[1], (interruptFlag_1or5 + TCIFx));

        if (I2C_STATE_BUSY == I2C_state_rx[I2C1])
        {
            // Set Buffer interrupt enable
            SET_BIT(I2C_reg[I2C1]->CR2, 10U);
            // DMA requests disable
            CLEAR_BIT(I2C_reg[I2C1]->CR2, 11U);
        }
    }

    if (READ_REG(DMA_reg[DMA1]->ISR[1], 1UL, (interruptFlag_1or5 + DMEIFx)))
    {
        SET_BIT(DMA_reg[DMA1]->IFCR[1], (interruptFlag_1or5 + DMEIFx));
    }

    if (READ_REG(DMA_reg[DMA1]->ISR[1], 1UL, (interruptFlag_1or5 + TEIFx)))
    {
        SET_BIT(DMA_reg[DMA1]->IFCR[1], (interruptFlag_1or5 + TEIFx));
    }
}

void DMA1_Stream6_Handler(void)
{
    if (READ_REG(DMA_reg[DMA1]->ISR[1], 1UL, (interruptFlag_2or6 + TCIFx)))
    {
        // Clear TCIFx
        SET_BIT(DMA_reg[DMA1]->IFCR[1], (interruptFlag_2or6 + TCIFx));

        if (I2C_STATE_BUSY == I2C_state_tx[I2C1])
        {
            // DMA requests disable
            CLEAR_BIT(I2C_reg[I2C1]->CR2, 11U);
        }
    }

    if (READ_REG(DMA_reg[DMA1]->ISR[1], 1UL, (interruptFlag_2or6 + DMEIFx)))
    {
        SET_BIT(DMA_reg[DMA1]->IFCR[1], (interruptFlag_2or6 + DMEIFx));
    }

    if (READ_REG(DMA_reg[DMA1]->ISR[1], 1UL, (interruptFlag_2or6 + TEIFx)))
    {
        SET_BIT(DMA_reg[DMA1]->IFCR[1], (interruptFlag_2or6 + TEIFx));
    }
}

void DMA1_Stream7_Handler(void)
{
    if (READ_REG(DMA_reg[DMA1]->ISR[1], 1UL, (interruptFlag_3or7 + TCIFx)))
    {
        // Clear TCIFx
        SET_BIT(DMA_reg[DMA1]->IFCR[1], (interruptFlag_3or7 + TCIFx));

        if (I2C_STATE_BUSY == I2C_state_tx[I2C1])
        {
            // DMA requests disable
            CLEAR_BIT(I2C_reg[I2C1]->CR2, 11U);
        }

        if (I2C_STATE_BUSY == I2C_state_tx[I2C2])
        {
            // DMA requests disable
            CLEAR_BIT(I2C_reg[I2C2]->CR2, 11U);
        }
    }

    if (READ_REG(DMA_reg[DMA1]->ISR[1], 1UL, (interruptFlag_3or7 + DMEIFx)))
    {
        SET_BIT(DMA_reg[DMA1]->IFCR[1], (interruptFlag_3or7 + DMEIFx));
    }

    if (READ_REG(DMA_reg[DMA1]->ISR[1], 1UL, (interruptFlag_3or7 + TEIFx)))
    {
        SET_BIT(DMA_reg[DMA1]->IFCR[1], (interruptFlag_3or7 + TEIFx));
    }
}

void DMA2_Stream0_Handler(void)
{
    if (READ_REG(DMA_reg[DMA2]->ISR[0], 1UL, (interruptFlag_0or4 + TCIFx)))
    {
        // Clear TCIFx
        SET_BIT(DMA_reg[DMA2]->IFCR[0], (interruptFlag_0or4 + TCIFx));
    }

    if (READ_REG(DMA_reg[DMA2]->ISR[0], 1UL, (interruptFlag_0or4 + DMEIFx)))
    {
        SET_BIT(DMA_reg[DMA2]->IFCR[0], (interruptFlag_0or4 + DMEIFx));
    }

    if (READ_REG(DMA_reg[DMA2]->ISR[0], 1UL, (interruptFlag_0or4 + TEIFx)))
    {
        SET_BIT(DMA_reg[DMA2]->IFCR[0], (interruptFlag_0or4 + TEIFx));
    }
}

void DMA2_Stream1_Handler(void)
{
    if (READ_REG(DMA_reg[DMA2]->ISR[0], 1UL, (interruptFlag_1or5 + TCIFx)))
    {
        // Clear TCIFx
        SET_BIT(DMA_reg[DMA2]->IFCR[0], (interruptFlag_1or5 + TCIFx));
    }

    if (READ_REG(DMA_reg[DMA2]->ISR[0], 1UL, (interruptFlag_1or5 + DMEIFx)))
    {
        SET_BIT(DMA_reg[DMA2]->IFCR[0], (interruptFlag_1or5 + DMEIFx));
    }

    if (READ_REG(DMA_reg[DMA2]->ISR[0], 1UL, (interruptFlag_1or5 + TEIFx)))
    {
        SET_BIT(DMA_reg[DMA2]->IFCR[0], (interruptFlag_1or5 + TEIFx));
    }
}

void DMA2_Stream2_Handler(void)
{
    if (READ_REG(DMA_reg[DMA2]->ISR[0], 1UL, (interruptFlag_2or6 + TCIFx)))
    {
        if (INITTED == UART_init_state[USART1])
        {
            if (false == UART_recv_buf[USART1].overrun)
            {
                UART_recv_buf[USART1].overrun = true;
            }
        }

        // Clear TCIFx
        SET_BIT(DMA_reg[DMA2]->IFCR[0], (interruptFlag_2or6 + TCIFx));
    }

    if (READ_REG(DMA_reg[DMA2]->ISR[0], 1UL, (interruptFlag_2or6 + DMEIFx)))
    {
        SET_BIT(DMA_reg[DMA2]->IFCR[0], (interruptFlag_2or6 + DMEIFx));
    }

    if (READ_REG(DMA_reg[DMA2]->ISR[0], 1UL, (interruptFlag_2or6 + TEIFx)))
    {
        SET_BIT(DMA_reg[DMA2]->IFCR[0], (interruptFlag_2or6 + TEIFx));
    }
}

void DMA2_Stream3_Handler(void)
{
    if (READ_REG(DMA_reg[DMA2]->ISR[0], 1UL, (interruptFlag_3or7 + TCIFx)))
    {
        // Clear TCIFx
        SET_BIT(DMA_reg[DMA2]->IFCR[0], (interruptFlag_3or7 + TCIFx));
    }

    if (READ_REG(DMA_reg[DMA2]->ISR[0], 1UL, (interruptFlag_3or7 + DMEIFx)))
    {
        SET_BIT(DMA_reg[DMA2]->IFCR[0], (interruptFlag_3or7 + DMEIFx));
    }

    if (READ_REG(DMA_reg[DMA2]->ISR[0], 1UL, (interruptFlag_3or7 + TEIFx)))
    {
        SET_BIT(DMA_reg[DMA2]->IFCR[0], (interruptFlag_3or7 + TEIFx));
    }
}

void DMA2_Stream4_Handler(void)
{
    if (READ_REG(DMA_reg[DMA2]->ISR[1], 1UL, (interruptFlag_0or4 + TCIFx)))
    {
        // Clear TCIFx
        SET_BIT(DMA_reg[DMA2]->IFCR[1], (interruptFlag_0or4 + TCIFx));
    }

    if (READ_REG(DMA_reg[DMA2]->ISR[1], 1UL, (interruptFlag_0or4 + DMEIFx)))
    {
        SET_BIT(DMA_reg[DMA2]->IFCR[1], (interruptFlag_0or4 + DMEIFx));
    }

    if (READ_REG(DMA_reg[DMA2]->ISR[1], 1UL, (interruptFlag_0or4 + TEIFx)))
    {
        SET_BIT(DMA_reg[DMA2]->IFCR[1], (interruptFlag_0or4 + TEIFx));
    }
}

void DMA2_Stream5_Handler(void)
{
    if (READ_REG(DMA_reg[DMA2]->ISR[1], 1UL, (interruptFlag_1or5 + TCIFx)))
    {
        // Clear TCIFx
        SET_BIT(DMA_reg[DMA2]->IFCR[1], (interruptFlag_1or5 + TCIFx));
    }

    if (READ_REG(DMA_reg[DMA2]->ISR[1], 1UL, (interruptFlag_1or5 + DMEIFx)))
    {
        SET_BIT(DMA_reg[DMA2]->IFCR[1], (interruptFlag_1or5 + DMEIFx));
    }

    if (READ_REG(DMA_reg[DMA2]->ISR[1], 1UL, (interruptFlag_1or5 + TEIFx)))
    {
        SET_BIT(DMA_reg[DMA2]->IFCR[1], (interruptFlag_1or5 + TEIFx));
    }
}

void DMA2_Stream6_Handler(void)
{
    if (READ_REG(DMA_reg[DMA2]->ISR[1], 1UL, (interruptFlag_2or6 + TCIFx)))
    {
        // Clear TCIFx
        SET_BIT(DMA_reg[DMA2]->IFCR[1], (interruptFlag_2or6 + TCIFx));
    }

    if (READ_REG(DMA_reg[DMA2]->ISR[1], 1UL, (interruptFlag_2or6 + DMEIFx)))
    {
        SET_BIT(DMA_reg[DMA2]->IFCR[1], (interruptFlag_2or6 + DMEIFx));
    }

    if (READ_REG(DMA_reg[DMA2]->ISR[1], 1UL, (interruptFlag_2or6 + TEIFx)))
    {
        SET_BIT(DMA_reg[DMA2]->IFCR[1], (interruptFlag_2or6 + TEIFx));
    }
}

void DMA2_Stream7_Handler(void)
{
    if (READ_REG(DMA_reg[DMA2]->ISR[1], 1UL, (interruptFlag_3or7 + TCIFx)))
    {
        // Clear TCIFx
        SET_BIT(DMA_reg[DMA2]->IFCR[1], (interruptFlag_3or7 + TCIFx));

        if (UART_STATE_BUSY == UART_state_tx[USART1])
        {
            UART_state_tx[USART1] = UART_STATE_READY;
            // Disable DMA transmitter
		    CLEAR_BIT(USART_reg[USART1]->CR3, 7U);
		    // Disable Transmitter
		    CLEAR_BIT(USART_reg[USART1]->CR1, 3U);
        }
    }

    if (READ_REG(DMA_reg[DMA2]->ISR[1], 1UL, (interruptFlag_3or7 + DMEIFx)))
    {
        SET_BIT(DMA_reg[DMA2]->IFCR[1], (interruptFlag_3or7 + DMEIFx));
    }

    if (READ_REG(DMA_reg[DMA2]->ISR[1], 1UL, (interruptFlag_3or7 + TEIFx)))
    {
        SET_BIT(DMA_reg[DMA2]->IFCR[1], (interruptFlag_3or7 + TEIFx));
    }
}

void TIM2_Handler(void)
{
    // User define
}

void TIM8_UP_TIM13_Handler(void)
{
    // User define
}

void SysTick_Handler(void)
{
    SysTick_cnt_u32 ++;
}

void Default_Handler(void)
{
    while (1)
    {
        // do nothing
    }
}

static inline void LMAtoVMA(uint32_t *pDst, uint32_t *pSrc, uint32_t *pDst_end)
{
    while (pDst < pDst_end)
    {
        *(pDst++) = *(pSrc++);
    }
}

static inline void clear_bss(uint32_t *pStart, uint32_t *pEnd)
{
    while (pStart < pEnd)
    {
        *(pStart++) = 0;
    }
}
