#include "GPIO.h"
#include "WWDG.h"
#include "Interrupt.h"

int main(void)
{
    uint32_t timegap_u32;
	
    GPIO_setup(GPIOAEN, 6U, GP_OUT, AF0, PP, PU);
    GPIO_OUT_setVal(GPIOAEN, 6U, 1);

    timegap_u32 = SysTick_cnt_u32;
	while (10UL > (SysTick_cnt_u32 - timegap_u32));

    GPIO_OUT_setVal(GPIOAEN, 6U, 0);
    WWDG_init(div_8, 128, 0);
	while (1);

    return 0;
}
