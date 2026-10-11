#include "GPIO.h"
#include "UART.h"

int main(void)
{
	uint8_t tx_data[3] = {48, 49, 50};
    uint8_t rx_data = 0;
    
	UART_init(USART1, 9600); // Initialize UART with a baudrate
	// Setup GPIO
	GPIO_setup(GPIOEEN, 4, IN, AF7, PP, PU);
	GPIO_setup(GPIOAEN, 6, GP_OUT, AF7, PP, PU);
    GPIO_OUT_setVal(GPIOAEN, 6, 1);

	while (1)
	{
		if (0 == GPIO_IN_getVal(GPIOEEN, 4))
		{
			UART_transmit(USART1, tx_data, 3);
		}
		else
		{
			// do nothing
		}		

		if (true == isUpdated_UART[USART1])
		{
			UART_Read(USART1, &rx_data, 1);

			if (48 == rx_data)
			{
				GPIO_OUT_setVal(GPIOAEN, 6, 0);
			}
			else
			{
				GPIO_OUT_setVal(GPIOAEN, 6, 1);
			}
		}
	}

    return 0;
}
