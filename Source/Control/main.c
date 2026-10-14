#include "I2C.h"
#include "GPIO.h"

int main(void)
{
    I2C_init(I2C1);
    GPIO_setup(GPIOEEN, 3, IN, AF0, PP, PU);
    GPIO_setup(GPIOAEN, 6, GP_OUT, AF0, PP, PU);

    while (1)
    {
        if (0 == GPIO_IN_getVal(GPIOEEN, 3))
        {
            GPIO_OUT_setVal(GPIOAEN, 6, 0);
            I2C_master_recv(I2C1, 104, 14);
        }
        else
        {
            GPIO_OUT_setVal(GPIOAEN, 6, 1);
        }
    }
    
    return 0;
}
