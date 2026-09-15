#include "bit_operator.h"
#include "FLASH_header.h"
#include "SCB_header.h"
#include "SYSTICK_header.h"
#include "RCC_header.h"
#include "PWR_header.h"
#include "FP_header.h"
#include "init_static.h"

#define VECTKEY_WRITE 0x05FAUL
#define VECTKEY_READ 0xFA05UL

unsigned int SysClock_Freq = HSI_VALUE;
unsigned int AHB_freq = HSI_VALUE;
unsigned int APB1_freq = 0U;
unsigned int APB2_freq = 0U;

static bool prioritySetup(priConf_t priConf_st);
static bool initTick(priConf_t priConf_st);
static uint32_t getPriorityVal(priGroup_t priGroupField, uint8_t groupPriority, uint8_t subPriority);
static bool oscSetup(oscillatorConf_t oscillatorConf_st);
static bool clockSetup(clockConf_t clockConf_st);
static uint8_t flashLatency_Setup(unsigned int SysClock_Freq_param);

bool initConfig(priConf_t priConf_st, oscillatorConf_t oscillatorConf_st, clockConf_t clockConf_st)
{
	// Full access for Coprocessor
	WRITE_REG(SCB_reg->CPACR, 0xFUL, 20U, 0xFUL);
	// Set ASPEN and LSPEN
	WRITE_REG(FP_reg->FPCCR, 0x3UL, 30U, 0x3UL);

	// DSB (Data Synchronization Barrier):
	// Make sure data written into SCB_reg->CPACR done 100%
	__asm__ volatile ("dsb 0xF" ::: "memory");
	// ISB (Instruction Synchronization Barrier):
	// Xóa sạch Pipeline của CPU, nạp lại các lệnh phía sau.
	// Đảm bảo các lệnh FPU tiếp theo nhận biết FPU đã bật.
	__asm__ volatile ("isb 0xF" ::: "memory");

	if (OK != oscSetup(oscillatorConf_st))
	{
		return NOT_OK;
	}
	else if (OK != clockSetup(clockConf_st))
	{
		return NOT_OK;
	}
	else
	{
		// do nothing
	}

	return prioritySetup(priConf_st);
}

static bool prioritySetup(priConf_t priConf_st)
{
	bool returnVal = OK;

	if (priConf_st.priGroupField > PRIGROUP_7)
	{
		return NOT_OK;
	}

	WRITE_REG(SCB_reg->AIRCR, 0xFFFFUL, 16U, VECTKEY_WRITE); // Write 0x5FA to VECTKEY for writting
	WRITE_REG(SCB_reg->AIRCR, 0x7UL, 8U, (uint32_t)priConf_st.priGroupField); // Set PRIGROUP
	WRITE_REG(SCB_reg->AIRCR, 0xFFFFUL, 16U, VECTKEY_READ); // Write 0xFA05 to VECTKEY for read only

	returnVal |= initTick(priConf_st);

	return returnVal;
}

static bool initTick(priConf_t priConf_st)
{
	unsigned int Systick_LOAD = AHB_freq / priConf_st.systickFreq - 1UL;
	
	if (Systick_LOAD > 0xFFFFFFUL)
	{
		return NOT_OK;
	}
	else
	{
		// do nothing
	}

	WRITE_REG(SYSTICK_reg->LOAD, 0xFFFFFFUL, 0U, Systick_LOAD);
	WRITE_REG(SCB_reg->SHPR3, 0xFFUL, 24U, getPriorityVal(priConf_st.priGroupField, 5, 0));
	WRITE_REG(SYSTICK_reg->VAL, 0xFFFFFFUL, 0U, 0UL); // clears the counter
	WRITE_REG(SYSTICK_reg->CTRL, 0x7UL, 0U, 7UL); // Enable Interrupt, Enable Counter, Use AHB clock

	return OK;
}

static uint32_t getPriorityVal(priGroup_t priGroupField, uint8_t groupPriority, uint8_t subPriority)
{
	uint32_t priorityVal = 0;

	switch (priGroupField)
	{
		case PRIGROUP_0:
		case PRIGROUP_1:
		case PRIGROUP_2:
		case PRIGROUP_3:

			if ((groupPriority > 15U) || (subPriority > 0U))
			{
				return NOT_OK;
			}
			else
			{
				// do nothing
			}

			priorityVal = (groupPriority << 4U);
			break;

		case PRIGROUP_4:

			if ((groupPriority > 7U) || (subPriority > 1U))
			{
				return NOT_OK;
			}
			else
			{
				// do nothing
			}

			priorityVal = (((groupPriority << 1U) | subPriority) << 4U);
			break;

		case PRIGROUP_5:

			if ((groupPriority > 3U) || (subPriority > 3U))
			{
				return NOT_OK;
			}
			else
			{
				// do nothing
			}

			priorityVal = (((groupPriority << 2U) | subPriority) << 4U);
			break;

		case PRIGROUP_6:

			if ((groupPriority > 1U) || (subPriority > 7U))
			{
				return NOT_OK;
			}
			else
			{
				// do nothing
			}

			priorityVal = (((groupPriority << 3U) | subPriority) << 4U);
			break;

		case PRIGROUP_7:

			if ((groupPriority > 0U) || (subPriority > 15U))
			{
				return NOT_OK;
			}
			else
			{
				// do nothing
			}

			priorityVal = (subPriority << 4U);
			break;
	}

	return priorityVal;
}

static bool oscSetup(oscillatorConf_t oscillatorConf_st)
{
	SET_BIT(RCC_reg->APB1ENR, 28U);	// Power interface clock enable
	// Set flash latency to a safe value before increasing clock speed.
	// The final value will be set after clock configuration.
	WRITE_REG(FLASH_reg->ACR, 0x7UL, 0U, (uint32_t)seven);

	switch (oscillatorConf_st.Oscillator)
	{
		case HSE:
			uint32_t timeout_counter = 0;
			SET_BIT(RCC_reg->CR, 16U); // Enable HSE clock

			while (!(READ_REG(RCC_reg->CR, 1UL, 17U))) // wait until HSE ready
			{
				timeout_counter++;
				if (timeout_counter > 0x10000) return NOT_OK; // Timeout exit
			}

			if (DISABLE == oscillatorConf_st.PLL.PLL_enable)
			{
				WRITE_REG(RCC_reg->CFGR, 3UL, 0U, 1UL); // choose HSE as system clock
				
				while (1UL != READ_REG(RCC_reg->CFGR, 3UL, 2U))	// wait until HSE is choose as system clock
				{
					//do nothing
				}

				SysClock_Freq = HSE_VALUE;
			}
			else
			{
				// do nothing
			}
			
			break;

		case HSI:
			SET_BIT(RCC_reg->CR, 0U); // Enable HSI clock

			while (!(READ_REG(RCC_reg->CR, 1UL, 1U))) // wait until HSI ready
			{
				// do nothing
			}

			if (oscillatorConf_st.HSITRIM > 0x1FUL)
			{
				return NOT_OK;
			}
			else
			{
				// do nothing
			}

			WRITE_REG(RCC_reg->CR, 0x1FUL, 3U, (uint32_t)oscillatorConf_st.HSITRIM); // Setup HSI Trim
			
			if (DISABLE == oscillatorConf_st.PLL.PLL_enable)
			{
				WRITE_REG(RCC_reg->CFGR, 0x3UL, 0U, 0UL); // choose HSI as system clock

				while (0UL != READ_REG(RCC_reg->CFGR, 3UL, 2U))	// wait until HSI is choose as system clock
				{
					// do nothing
				}

				SysClock_Freq = HSI_VALUE;
			}
			else
			{
				// do nothing
			}

			break;

		case LSI:
			SET_BIT(RCC_reg->CSR, 0U); // Enable LSI clock

			while (!(READ_REG(RCC_reg->CSR, 1UL, 1U))) // wait until LSI ready
			{
				// do nothing
			}

			SysClock_Freq = LSI_VALUE;
			break;

		case LSE:

			if (!(READ_REG(PWR_reg->CR, 1UL, 8U)))
			{
				SET_BIT(PWR_reg->CR, 8U); // Enable access to RTC and RTC Backup registers and backup SRAM
			}
			else
			{
				// do nothing
			}
			
			while (!(READ_REG(PWR_reg->CR, 1UL, 8U)))
			{
				// do nothing
			}
			
			SET_BIT(RCC_reg->BDCR, 0U); // Enable LSE clock

			while (!(READ_REG(RCC_reg->BDCR, 1UL, 1U))) // wait until LSE ready
			{
				// do nothing
			}

			SysClock_Freq = LSE_VALUE;
			break;

		default:
			return NOT_OK;
			break;
	}

	if (oscillatorConf_st.PLL.PLL_enable == ENABLE)
	{
		unsigned int tmp_SysClock_Freq = 0;

		CLEAR_BIT(RCC_reg->CR, 24U); // Disable PLL

		while (READ_REG(RCC_reg->CR, 1UL, 25U)) // Wait until PLL is unlocked
		{
			// do nothing
		}

		if ((uint8_t)0x1F < oscillatorConf_st.PLL.PLLM)
		{
			return NOT_OK;
		}
		else
		{
			// do nothing
		}

		if ((uint16_t)0x1FF < oscillatorConf_st.PLL.PLLN)
		{
			return NOT_OK;
		}
		else
		{
			// do nothing
		}
		
		if ((uint8_t)0x3 < oscillatorConf_st.PLL.PLLP)
		{
			return NOT_OK;
		}
		else
		{
			// do nothing
		}
		
		if ((uint8_t)0xF < oscillatorConf_st.PLL.PLLQ)
		{
			return NOT_OK;
		}
		else
		{
			// do nothing
		}

		if (HSI == oscillatorConf_st.Oscillator)
		{
			CLEAR_BIT(RCC_reg->PLLCFGR, 22U); // choose HSI as clock source for PLL
			tmp_SysClock_Freq = ((unsigned int)HSI_VALUE / oscillatorConf_st.PLL.PLLM) * oscillatorConf_st.PLL.PLLN / oscillatorConf_st.PLL.PLLP;
		}
		else if (HSE == oscillatorConf_st.Oscillator)
		{
			SET_BIT(RCC_reg->PLLCFGR, 22U); // choose HSE as clock source for PLL
			tmp_SysClock_Freq = ((unsigned int)HSE_VALUE / oscillatorConf_st.PLL.PLLM) * oscillatorConf_st.PLL.PLLN / oscillatorConf_st.PLL.PLLP;
		}
		else
		{
			return NOT_OK;
		}

		RCC_reg->PLLCFGR |= ((uint32_t)oscillatorConf_st.PLL.PLLM | ((uint32_t)oscillatorConf_st.PLL.PLLN << 6U) | ((uint32_t)oscillatorConf_st.PLL.PLLP << 16U) | ((uint32_t)oscillatorConf_st.PLL.PLLQ << 24U));
		SET_BIT(RCC_reg->CR, 24U); // Enable PLL

		while (!(READ_REG(RCC_reg->CR, 1UL, 25U))) // Wait for PLL to be ready
		{
			// do nothing
		}

		WRITE_REG(RCC_reg->CFGR, 0x3UL, 0U, 2UL); // choose PLL as system clock

		while (2UL != READ_REG(RCC_reg->CFGR, 3UL, 2U)) // Wait for PLL to be system clock
		{
			// do nothing
		}

		SysClock_Freq = tmp_SysClock_Freq;
	}
	else
	{
		// do nothing
	}

	return OK;
}

static bool clockSetup(clockConf_t clockConf_st)
{
	switch (clockConf_st.AHB_prescaler)
	{
		case AHB_not_divided:
			WRITE_REG(RCC_reg->CFGR, 0xFUL, 4U, 0UL);
			AHB_freq = SysClock_Freq;
			break;
		case AHB_divided_by_2:
			WRITE_REG(RCC_reg->CFGR, 0xFUL, 4U, 8UL);
			AHB_freq = SysClock_Freq / 2U;
			break;
		case AHB_divided_by_4:
			WRITE_REG(RCC_reg->CFGR, 0xFUL, 4U, 9UL);
			AHB_freq = SysClock_Freq / 4U;
			break;
		case AHB_divided_by_8:
			WRITE_REG(RCC_reg->CFGR, 0xFUL, 4U, 10UL);
			AHB_freq = SysClock_Freq / 8U;
			break;
		case AHB_divided_by_16:
			WRITE_REG(RCC_reg->CFGR, 0xFUL, 4U, 11UL);
			AHB_freq = SysClock_Freq / 16U;
			break;
		case AHB_divided_by_64:
			WRITE_REG(RCC_reg->CFGR, 0xFUL, 4U, 12UL);
			AHB_freq = SysClock_Freq / 64U;
			break;
		case AHB_divided_by_128:
			WRITE_REG(RCC_reg->CFGR, 0xFUL, 4U, 13UL);
			AHB_freq = SysClock_Freq / 128U;
			break;
		case AHB_divided_by_256:
			WRITE_REG(RCC_reg->CFGR, 0xFUL, 4U, 14UL);
			AHB_freq = SysClock_Freq / 256U;
			break;
		case AHB_divided_by_512:
			WRITE_REG(RCC_reg->CFGR, 0xFUL, 4U, 15UL);
			AHB_freq = SysClock_Freq / 512U;
			break;
		default:
			break;
	}

	switch (clockConf_st.PPRE1)
	{
		case APBx_not_divided:
			WRITE_REG(RCC_reg->CFGR, 0x7UL, 10U, 0UL);
			APB1_freq = AHB_freq;
			break;
		case APBx_divided_by_2:
			WRITE_REG(RCC_reg->CFGR, 0x7UL, 10U, 4UL);
			APB1_freq = AHB_freq / 2U;
			break;
		case APBx_divided_by_4:
			WRITE_REG(RCC_reg->CFGR, 0x7UL, 10U, 5UL);
			APB1_freq = AHB_freq / 4U;
			break;
		case APBx_divided_by_8:
			WRITE_REG(RCC_reg->CFGR, 0x7UL, 10U, 6UL);
			APB1_freq = AHB_freq / 8U;
			break;
		case APBx_divided_by_16:
			WRITE_REG(RCC_reg->CFGR, 0x7UL, 10U, 7UL);
			APB1_freq = AHB_freq / 16U;
			break;
		default:
			break;
	}

	switch (clockConf_st.PPRE2)
	{
		case APBx_not_divided:
			WRITE_REG(RCC_reg->CFGR, 0x7UL, 13U, 0UL);
			APB2_freq = AHB_freq;
			break;
		case APBx_divided_by_2:
			WRITE_REG(RCC_reg->CFGR, 0x7UL, 13U, 4UL);
			APB2_freq = AHB_freq / 2U;
			break;
		case APBx_divided_by_4:
			WRITE_REG(RCC_reg->CFGR, 0x7UL, 13U, 5UL);
			APB2_freq = AHB_freq / 4U;
			break;
		case APBx_divided_by_8:
			WRITE_REG(RCC_reg->CFGR, 0x7UL, 13U, 6UL);
			APB2_freq = AHB_freq / 8U;
			break;
		case APBx_divided_by_16:
			WRITE_REG(RCC_reg->CFGR, 0x7UL, 13U, 7UL);
			APB2_freq = AHB_freq / 16U;
			break;
		default:
			break;
	}

	if (144000000UL <= APB1_freq)
	{
		SET_BIT(PWR_reg->CR, 14U);
	}
	else
	{
		CLEAR_BIT(PWR_reg->CR, 14U);
	}

	WRITE_REG(FLASH_reg->ACR, 7UL, 0U, (uint32_t)flashLatency_Setup((unsigned int)AHB_freq));
	WRITE_REG(FLASH_reg->ACR, 7UL, 8U, 7UL); // Enable PRFTEN, ICEN, DCEN
	
	return OK;
}

static uint8_t flashLatency_Setup(unsigned int SysClock_Freq_param)
{
	unsigned int flashLatency = 0;

	if (SysClock_Freq_param <= 30000000U)
	{
		flashLatency = zero;
	}
	else if (SysClock_Freq_param <= 60000000U)
	{
		flashLatency = one;
	}
	else if (SysClock_Freq_param <= 90000000U)
	{
		flashLatency = two;
	}
	else if (SysClock_Freq_param <= 120000000U)
	{
		flashLatency = three;
	}
	else if (SysClock_Freq_param <= 150000000U)
	{
		flashLatency = four;
	}
	else if (SysClock_Freq_param <= 168000000U)
	{
		flashLatency = five;
	}
	else
	{
		flashLatency = seven; // Max latency for safety if frequency is out of expected range
	}

	return flashLatency;
}
