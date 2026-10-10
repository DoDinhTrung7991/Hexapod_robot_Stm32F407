#include "GPIO.h"
#include "DMA.h"
// #include "UART.h"

int main(void)
{
	// uint8_t tx_data[3] = {48, 49, 50};
    // uint8_t rx_data = 0;
    
	// UART_init(USART1, 9600); // Initialize UART with a baudrate
	// // Setup GPIO
	// GPIO_setup(GPIOEEN, 4, IN, AF7, PP, PU);
	// GPIO_setup(GPIOAEN, 6, GP_OUT, AF7, PP, PU);
    // GPIO_OUT_setVal(GPIOAEN, 6, 1);

	// while (1)
	// {
	// 	if (0 == GPIO_IN_getVal(GPIOEEN, 4))
	// 	{
	// 		UART_transmit(USART1, tx_data, 3);
	// 	}
	// 	else
	// 	{
	// 		// do nothing
	// 	}		

	// 	if (true == isUpdated_UART[USART1])
	// 	{
	// 		UART_Read(USART1, &rx_data, 1);

	// 		if (48 == rx_data)
	// 		{
	// 			GPIO_OUT_setVal(GPIOAEN, 6, 0);
	// 		}
	// 		else
	// 		{
	// 			GPIO_OUT_setVal(GPIOAEN, 6, 1);
	// 		}
	// 	}
	// }

	uint8_t send_buf[8] = {0, 1, 2, 3, 4, 5, 6, 7};
	uint8_t recv_buf[8] = {0};

	DMA_direct_param_t DMA_direct_param_st = (DMA_direct_param_t)
	{
		{
			.DMAx = DMA2,
			.stream = Stream_0,
			.channel = 1
		},
		{
			.double_buffer_en = disable,
			.circular_mode_en = disable,
			.peri_data_size = byte,
			.mem_data_size = byte,
			.peri_mode = not_fixed,
			.mem_mode = not_fixed
		},
		{
			.dir = mem_to_mem,
			.flow_controller = DMA
		},
		{
			.stream_priority = low,
			.interrupt_en_u8 = (TCIE | DMEIE)
		}
	};

	buffer_t buffer_info_st =
	{
		.data_length = 8,
		.peri_addr = (volatile uint32_t *)send_buf,
		.mem_addr = (volatile uint32_t *)recv_buf
	};

	// Setup GPIO input
	GPIO_setup(GPIOEEN, 4, IN, AF7, PP, PU);
	// Setup GPIO output
	GPIO_setup(GPIOAEN, 6, GP_OUT, AF7, PP, PU);
	GPIO_setup(GPIOAEN, 7, GP_OUT, AF7, PP, PU);

	DMA_direct_init(DMA_direct_param_st);
	DMA_transfer(DMA_direct_param_st.Stream_info_st, buffer_info_st);

	while (1)
	{
		if (0 == GPIO_IN_getVal(GPIOEEN, 4))
		{
			GPIO_OUT_setVal(GPIOAEN, 7, 0);
			
			if (recv_buf[1] > 0)
			{
				GPIO_OUT_setVal(GPIOAEN, 6, 0);
			}
			else
			{
				GPIO_OUT_setVal(GPIOAEN, 6, 1);
			}
		}
		else
		{
			GPIO_OUT_setVal(GPIOAEN, 6, 1);
			GPIO_OUT_setVal(GPIOAEN, 7, 1);
		}
	}

    return 0;
}
