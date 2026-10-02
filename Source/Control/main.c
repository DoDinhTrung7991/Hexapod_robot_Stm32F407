#include "Interrupt.h"

int main(void)
{
	// input
	GPIO_setup(GPIOEEN, 4, IN, AF0, None, PU);
	GPIO_setup(GPIOEEN, 3, IN, AF0, None, PU);
	// output
	GPIO_setup(GPIOAEN, 6, GP_OUT, AF0, None, PU);
	GPIO_setup(GPIOAEN, 7, GP_OUT, AF0, None, PU);

	Ex_Interrupt(GPIOEEN, 4, Falling_Edge);
	Ex_Interrupt(GPIOEEN, 3, Falling_Edge);

    while (1)
    {
		
	}

    return 0;
}
