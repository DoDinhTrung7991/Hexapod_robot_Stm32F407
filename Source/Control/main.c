#include "Timer.h"

int main(void)
{
	PWM_init(TIM3, CHANN_1, 50);
	PWM_init(TIM3, CHANN_2, 50);

	PWM_Generation(TIM3, CHANN_1, 50);
	PWM_Generation(TIM3, CHANN_2, 50);

    while (1)
    {
		
	}

    return 0;
}
