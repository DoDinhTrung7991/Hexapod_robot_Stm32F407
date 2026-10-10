#include "RCC_header.h"
#include "bit_operator.h"
#include "UART.h"
#include "GPIO.h"
#include "Interrupt.h"

typedef struct
{
    GPIO_ENABLE_t GPIOx_en;
    uint8_t pos_u8;
	AFRx_t AFx_en;
} GPIO_UART_t;

USART_t * const USART_reg[6] = {
	(USART_t*)0x40011000,
	(USART_t*)0x40004400,
	(USART_t*)0x40004800,
	(USART_t*)0x40004C00,
	(USART_t*)0x40005000,
	(USART_t*)0x40011400
};

volatile queue_t UART_recv_buf[6];

__attribute__((section(".ccmram_data")))volatile uint8_t isUpdated_UART[6] = {
	false,
	false,
	false,
	false,
	false,
	false
};

__attribute__((section(".ccmram_data")))volatile bool UART_init_state[6] = {
	NOT_INITTED,
	NOT_INITTED,
	NOT_INITTED,
	NOT_INITTED,
	NOT_INITTED,
	NOT_INITTED
};

__attribute__((section(".ccmram_data")))volatile bool UART_state_tx[6] = {
	UART_STATE_READY,
	UART_STATE_READY,
	UART_STATE_READY,
	UART_STATE_READY,
	UART_STATE_READY,
	UART_STATE_READY
};

__attribute__((section(".ccmram_data")))volatile bool UART_state_rx[6] = {
	UART_STATE_READY,
	UART_STATE_READY,
	UART_STATE_READY,
	UART_STATE_READY,
	UART_STATE_READY,
	UART_STATE_READY
};

static const stream_channel_t UART_Stream_info_tx_st[6] =
{
	// USART1
	{
		.DMAx = DMA2,
		.stream = Stream_7,
		.channel = 4
	},
	// USART2
	{
		.DMAx = DMA1,
		.stream = Stream_6,
		.channel = 4
	},
	// USART3
	{
		.DMAx = DMA1,
		.stream = Stream_3,	// Stream_4
		.channel = 4	// 7
	},
	// UART4
	{
		.DMAx = DMA1,
		.stream = Stream_4,
		.channel = 4
	},
	//UART5
	{
		.DMAx = DMA1,
		.stream = Stream_7,
		.channel = 4
	},
	// USART6
	{
		.DMAx = DMA2,
		.stream = Stream_7, // Stream_6
		.channel = 5
	}
};

static const stream_channel_t UART_Stream_info_rx_st[6] =
{
	// USART1
	{
		.DMAx = DMA2,
		.stream = Stream_2,	// Stream_5
		.channel = 4
	},
	// USART2
	{
		.DMAx = DMA1,
		.stream = Stream_5,
		.channel = 4
	},
	// USART3
	{
		.DMAx = DMA1,
		.stream = Stream_1,
		.channel = 4
	},
	// UART4
	{
		.DMAx = DMA1,
		.stream = Stream_2,
		.channel = 4
	},
	//UART5
	{
		.DMAx = DMA1,
		.stream = Stream_0,
		.channel = 4
	},
	// USART6
	{
		.DMAx = DMA2,
		.stream = Stream_2, // Stream_1
		.channel = 5
	}
};

static const GPIO_UART_t GPIO_UART_tx_st[6] =
{
	{GPIOAEN, 9U, AF7},		// {GPIOBEN, 6U, AF7},
    {GPIOAEN, 2U, AF7},		// {GPIODEN, 5U, AF7},
    {GPIOBEN, 10U, AF7},	// {GPIOCEN, 10U, AF7},
	{GPIOAEN, 0U, AF8},		// {GPIOCEN, 10U, AF7},
    {GPIOCEN, 12U, AF8},
    {GPIOCEN, 6U, AF8}
};

static const GPIO_UART_t GPIO_UART_rx_st[6] =
{
	{GPIOAEN, 10U, AF7},	// {GPIOBEN, 7U, AF7},
    {GPIOAEN, 3U, AF7},		// {GPIODEN, 6U, AF7},
    {GPIOBEN, 11U, AF7},	// {GPIOCEN, 11U, AF7},
	{GPIOAEN, 1U, AF8},		// {GPIOCEN, 11U, AF7},
    {GPIODEN, 2U, AF8},
    {GPIOCEN, 7U, AF8}
};

static RCC_APBxENR_enable_t RCC_APBxENR_UART_arr[6] =
{
	{&RCC_reg->APB2ENR, 4U},
	{&RCC_reg->APB1ENR, 17U},
	{&RCC_reg->APB1ENR, 18U},
	{&RCC_reg->APB1ENR, 19U},
	{&RCC_reg->APB1ENR, 20U},
	{&RCC_reg->APB2ENR, 5U}
};

static const peripheral_Selection_t UARTx_Interrupt_line[6] =
{
	USART1_Interrupt,
	USART2_Interrupt,
	USART3_Interrupt,
	UART4_Interrupt,
	UART5_Interrupt,
	USART6_Interrupt
};

bool UART_init(UARTx_t UARTx, uint32_t baudrate_u32)
{
	UART_init_state[UARTx] = NOT_INITTED;
	static bool is_Init_done_once[6] = {false, false, false, false, false, false};

	if ((UART_STATE_READY == UART_state_tx[UARTx]) && (UART_STATE_READY == UART_state_rx[UARTx]))
	{
		uint32_t over8_u32;
		uint32_t brr_val_u32 = 0;
		uint32_t DIV_Mantissa_u32;
		uint32_t DIV_Fraction_u32;
		uint32_t fck_u32 = 0;

		if (false == is_Init_done_once[UARTx])
		{
			DMA_direct_param_t DMA_direct_param_tx_st = (DMA_direct_param_t)
			{
				{
					.DMAx = UART_Stream_info_tx_st[UARTx].DMAx,
					.stream = UART_Stream_info_tx_st[UARTx].stream,
					.channel = UART_Stream_info_tx_st[UARTx].channel
				},
				{
					.double_buffer_en = disable,
					.circular_mode_en = disable,
					.peri_data_size = byte,
					.mem_data_size = byte,
					.peri_mode = fixed,
					.mem_mode = not_fixed
				},
				{
					.dir = mem_to_peri,
					.flow_controller = DMA
				},
				{
					.stream_priority = low,
					.interrupt_en_u8 = (TCIE | DMEIE)
				}
			};
		
			DMA_direct_param_t DMA_direct_param_rx_st = (DMA_direct_param_t)
			{
				{
					.DMAx = UART_Stream_info_rx_st[UARTx].DMAx,
					.stream = UART_Stream_info_rx_st[UARTx].stream,
					.channel = UART_Stream_info_rx_st[UARTx].channel
				},
				{
					.double_buffer_en = disable,
					.circular_mode_en = enable,
					.peri_data_size = byte,
					.mem_data_size = byte,
					.peri_mode = fixed,
					.mem_mode = not_fixed
				},
				{
					.dir = peri_to_mem,
					.flow_controller = DMA
				},
				{
					.stream_priority = medium,
					.interrupt_en_u8 = (TCIE | DMEIE)
				}
			};
		
			buffer_t buffer_info_rx_st = (buffer_t)
			{
				.data_length = sizeof(UART_recv_buf[UARTx].buf),
				.peri_addr = &USART_reg[UARTx]->DR,
				.mem_addr = (volatile uint32_t *)UART_recv_buf[UARTx].buf
			};
			
			// Setting for DMA
			if ((NOT_OK == DMA_direct_init(DMA_direct_param_tx_st)) || (NOT_OK == DMA_direct_init(DMA_direct_param_rx_st)))
			{
				return NOT_OK;
			}
		
			DMA_transfer(DMA_direct_param_rx_st.Stream_info_st, buffer_info_rx_st);

			// Setup GPIO for USART ports
			GPIO_setup(GPIO_UART_tx_st[UARTx].GPIOx_en, GPIO_UART_tx_st[UARTx].pos_u8, AF, GPIO_UART_tx_st[UARTx].AFx_en, PP, PU);	// TX
			GPIO_setup(GPIO_UART_rx_st[UARTx].GPIOx_en, GPIO_UART_rx_st[UARTx].pos_u8, AF, GPIO_UART_rx_st[UARTx].AFx_en, PP, PU);	// RX

			// Setup EXTI to detect Start bit of UART Rx
			Ex_Interrupt(
				GPIO_UART_rx_st[UARTx].GPIOx_en,
				GPIO_UART_rx_st[UARTx].pos_u8,
				Falling_Edge
			);

			// Enable clock for USART peripheral
			SET_BIT(*RCC_APBxENR_UART_arr[UARTx].reg_u32_ptr, RCC_APBxENR_UART_arr[UARTx].pos);
			// Enable Interrupt line
			NVIC_ISER_setVal(UARTx_Interrupt_line[UARTx]);

			is_Init_done_once[UARTx] = true;
		}
		
		// Disable USART
		CLEAR_BIT(USART_reg[UARTx]->CR1, 13U);
		// Setting Data layout
		CLEAR_BIT(USART_reg[UARTx]->CR1, 12U);
		// Setting Stop bits
		WRITE_REG(USART_reg[UARTx]->CR2, 3UL, 12U, 0UL);
		// Choose oversampling
		CLEAR_BIT(USART_reg[UARTx]->CR1, 15U);
		
		// Calculate Baudrate
		over8_u32 = READ_REG(USART_reg[UARTx]->CR1, 1UL, 15U);
		
		if ((USART1 == UARTx) || (USART6 == UARTx))
		{
			fck_u32 = APB2_freq;
		}
		else if ((USART2 == UARTx) || (USART3 == UARTx) || (UART4 == UARTx) || (UART5 == UARTx))
		{
			fck_u32 = APB1_freq;
		}
		else
		{
			return NOT_OK;
		}

		DIV_Mantissa_u32 = fck_u32 / (8 * (2 - over8_u32) * baudrate_u32);
											//DIV_Fraction_u32 times 100
		DIV_Fraction_u32 = ((((fck_u32 * 1000) / (8 * (2 - over8_u32) * baudrate_u32)) % 1000) * 8 * (2 - over8_u32) + 500) / 1000;

		if (over8_u32)
		{
			if (8 <= DIV_Fraction_u32)
			{
				DIV_Fraction_u32 = 0;
				DIV_Mantissa_u32 ++;
			}
			else
			{
				// do nothing
			}
		}
		else
		{
			if (16 <= DIV_Fraction_u32)
			{
				DIV_Fraction_u32 = 0;
				DIV_Mantissa_u32 ++;
			}
			else
			{
				// do nothing
			}
		}

		brr_val_u32 = (DIV_Mantissa_u32 << 4U) | (DIV_Fraction_u32 & 0xFUL);
		// Setting Baudrate
		WRITE_REG(USART_reg[UARTx]->BRR, 0xFFFFUL, 0U, brr_val_u32);
		// Enable/Disable Parity control
		CLEAR_BIT(USART_reg[UARTx]->CR1, 10U);
		// CTS enable
		CLEAR_BIT(USART_reg[UARTx]->CR3, 9U);
		// RTS enable
		CLEAR_BIT(USART_reg[UARTx]->CR3, 8U);
		// Setting clock from CK pin for Synchronize mode
		CLEAR_BIT(USART_reg[UARTx]->CR2, 11U);
		// Half-duplex selection
		CLEAR_BIT(USART_reg[UARTx]->CR3, 3U);
		// LIN mode disable
		CLEAR_BIT(USART_reg[UARTx]->CR2, 14U);
		// Smartcard mode disable
		CLEAR_BIT(USART_reg[UARTx]->CR3, 5U);
		// IrDA mode disable
		CLEAR_BIT(USART_reg[UARTx]->CR3, 1U);

		// Enable/Disable interrupt
		// Enable Error interrupt
		SET_BIT(USART_reg[UARTx]->CR3, 0U);
		// Parity error interrupt disable
		CLEAR_BIT(USART_reg[UARTx]->CR1, 8U);
		// IDLE interrupt enable
		SET_BIT(USART_reg[UARTx]->CR1, 4U);
		// TXE interrupt disable
		CLEAR_BIT(USART_reg[UARTx]->CR1, 7U);
		// TC interrupt disable
		CLEAR_BIT(USART_reg[UARTx]->CR1, 6U);
		// RXNE interrupt disable
		CLEAR_BIT(USART_reg[UARTx]->CR1, 5U);

		// init queue
		queue_init((queue_t*)&UART_recv_buf[UARTx]);
		// Enable DMA receiver
		SET_BIT(USART_reg[UARTx]->CR3, 6U);
		// Enable Receiver
		SET_BIT(USART_reg[UARTx]->CR1, 2U);
		// Enable USART
		SET_BIT(USART_reg[UARTx]->CR1, 13U);
		UART_init_state[UARTx] = INITTED;
	}
	else
	{
		return NOT_OK;
	}

	return OK;
}

bool UART_transmit(UARTx_t UARTx, const uint8_t *buf, uint8_t data_length)
{
	if (
		(NOT_INITTED == UART_init_state[UARTx])
		|| (UART_STATE_BUSY == UART_state_tx[UARTx])
		|| (NULL == buf)
		|| (0 == data_length) 
		|| ((USART1 > UARTx) || (USART6 < UARTx))
	)
	{
		return NOT_OK;
	}
	else
	{
		DMA_transfer
		(
			UART_Stream_info_tx_st[UARTx], 
			(buffer_t){
				.data_length = data_length,
				.peri_addr = &USART_reg[UARTx]->DR,
				.mem_addr = (volatile uint32_t *)buf
			}
		);
		
		// Enable DMA transmitter
		SET_BIT(USART_reg[UARTx]->CR3, 7U);
		// Enable Transmitter
		SET_BIT(USART_reg[UARTx]->CR1, 3U);
		UART_state_tx[UARTx] = UART_STATE_BUSY;
	}

	return OK;
}

bool UART_Read(UARTx_t UARTx, uint8_t *buf, uint8_t data_length)
{
	if (true == UART_recv_buf[UARTx].isEmpty)
	{
		return NOT_OK;
	}
	else
	{
		uint8_t front_temp_u8;
		uint8_t index_u8 = 0;

		// Disable DMA Stream
		CLEAR_BIT(DMA_reg[UART_Stream_info_rx_st[UARTx].DMAx]->S[UART_Stream_info_rx_st[UARTx].stream].CR, 0U);
		while (READ_REG(DMA_reg[UART_Stream_info_rx_st[UARTx].DMAx]->S[UART_Stream_info_rx_st[UARTx].stream].CR, 1UL, 0U));
		// Disable Interrupt
		NVIC_ICER_setVal(UARTx_Interrupt_line[UARTx]);

		if (data_length > sizeof(UART_recv_buf[UARTx].buf))
		{
			data_length = sizeof(UART_recv_buf[UARTx].buf);
		}
		else
		{
			//do nothing
		}

		if (
			(UART_recv_buf[UARTx].front <= UART_recv_buf[UARTx].rear)
			&& (data_length <= (UART_recv_buf[UARTx].rear - UART_recv_buf[UARTx].front + 1))
		)
		{
			front_temp_u8 = UART_recv_buf[UARTx].rear + 1 - data_length;
		}
		else if (
			(true == UART_recv_buf[UARTx].overrun)
			&& (data_length < (sizeof(UART_recv_buf[UARTx].buf) - UART_recv_buf[UARTx].front + UART_recv_buf[UARTx].rear + 1))
		)
		{
			front_temp_u8 = (sizeof(UART_recv_buf[UARTx].buf) + UART_recv_buf[UARTx].rear + 1 - data_length) % sizeof(UART_recv_buf[UARTx].buf);
		}
		else
		{
			front_temp_u8 = UART_recv_buf[UARTx].front;
		}

		while (1)
		{
			buf[index_u8] = UART_recv_buf[UARTx].buf[front_temp_u8];

			if (front_temp_u8 == UART_recv_buf[UARTx].rear)
			{
				break;
			}

			front_temp_u8 = (front_temp_u8 + 1) % sizeof(UART_recv_buf[UARTx].buf);
			index_u8 ++;
		}
		
		UART_recv_buf[UARTx].front = (UART_recv_buf[UARTx].rear + 1) % sizeof(UART_recv_buf[UARTx].buf);
		UART_recv_buf[UARTx].overrun = false;
		UART_recv_buf[UARTx].isEmpty = true;
		UART_recv_buf[UARTx].isFull = false;
		isUpdated_UART[UARTx] = false;

		//Enable Stream
		SET_BIT(DMA_reg[UART_Stream_info_rx_st[UARTx].DMAx]->S[UART_Stream_info_rx_st[UARTx].stream].CR, 0U);
		// Enable Interrupt
		NVIC_ISER_setVal(UARTx_Interrupt_line[UARTx]);
	}

	return OK;
}
