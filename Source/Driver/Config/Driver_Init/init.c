#include "init_static.h"

bool SystemInit(void)
{
	bool returnVal = OK;

	priConf_t priConf_st = {
		.priGroupField = PRIGROUP_4, // 3 bits for group priority, 1 for sub-priority
		.systickFreq = 100U // SysTick_cnt_u32 = 10ms/cycle
	};
	
	oscillatorConf_t oscillatorConf_st = {
		.Oscillator = HSE,
		.HSITRIM = 16U, // Default trim value
		.PLL = {.PLL_enable = DISABLE}
	};

	clockConf_t clockConf_st = {
		.AHB_prescaler = AHB_not_divided,
		.PPRE1 = APBx_not_divided,
		.PPRE2 = APBx_not_divided
	};

	returnVal = initConfig(priConf_st, oscillatorConf_st, clockConf_st);

	return returnVal;
}
