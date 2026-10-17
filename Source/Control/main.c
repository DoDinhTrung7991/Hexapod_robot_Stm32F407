#include "I2C.h"
#include "GPIO.h"

int main(void)
{
    static bool once = 0;
    uint8_t I2C_send_arr[1] = {0x75};

    I2C_init(I2C1);
    GPIO_setup(GPIOEEN, 3, IN, AF0, PP, PU);

    I2C_master_send(I2C1, 104, I2C_send_arr, 1);

    while (1)
    {
        if (0 == GPIO_IN_getVal(GPIOEEN, 3))
        {
            if (0 == once)
            {
                I2C_master_recv(I2C1, 104, 2);
                once = 1;
            }
        }
        else
        {
            // do nothing
        }
    }
    
    return 0;
}
