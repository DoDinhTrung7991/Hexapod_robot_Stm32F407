#include "RCC_header.h"
#include "bit_operator.h"
#include "UART.h"
#include "GPIO.h"
#include "Interrupt.h"
#include <string.h>

typedef struct
{
    GPIO_ENABLE_t GPIOx_en;
    uint8_t pos_u8;
	AFRx_t AFx_en;
} GPIO_UART_t;

typedef struct
{
	volatile uint32_t *reg_u32_ptr;
	unsigned int pos;
} RCC_APBxENR_UART_t;

USART_t *USART_reg[6] = {
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

static RCC_APBxENR_UART_t RCC_APBxENR_UART_arr[6] =
{
	{&RCC_reg->APB2ENR, 4U},
	{&RCC_reg->APB1ENR, 17U},
	{&RCC_reg->APB1ENR, 18U},
	{&RCC_reg->APB1ENR, 19U},
	{&RCC_reg->APB1ENR, 20U},
	{&RCC_reg->APB2ENR, 5U}
};

static const unsigned int UARTx_Interrupt_line[6] =
{
	USART1_Interrupt,
	USART2_Interrupt,
	USART3_Interrupt,
	UART4_Interrupt,
	UART5_Interrupt,
	USART6_Interrupt
};

bool UART_init(UARTx_t UARTx, uint32_t baudrate)
{
	UART_init_state[UARTx] = NOT_INITTED;
	static bool is_Init_done_once[6] = {false, false, false, false, false, false};

	if ((UART_STATE_READY == UART_state_tx[UARTx]) || (UART_STATE_READY == UART_state_rx[UARTx]))
	{
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
					.peri_data_size = half_word,
					.mem_data_size = half_word,
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
					.peri_data_size = half_word,
					.mem_data_size = half_word,
					.peri_mode = fixed,
					.mem_mode = circular
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
				.data_length = ARR_SIZE,
				.peri_addr = &USART_reg[UARTx]->DR,
				.mem_addr = UART_recv_buf[UARTx].buf
			};
			
			// Setting for DMA
			if ((NOT_OK == DMA_direct_init(DMA_direct_param_tx_st)) || (NOT_OK == DMA_direct_init(DMA_direct_param_rx_st)))
			{
				return NOT_OK;
			}
		
			DMA_transfer(DMA_direct_param_rx_st.Stream_info_st, buffer_info_rx_st);

			// Setup GPIO for USART ports
			GPIO_setup(GPIO_UART_tx_st[UARTx].GPIOx_en, GPIO_UART_tx_st[UARTx].pos_u8, AF, GPIO_UART_tx_st[UARTx].AFx_en, PP, NoP);	// TX
			GPIO_setup(GPIO_UART_rx_st[UARTx].GPIOx_en, GPIO_UART_rx_st[UARTx].pos_u8, AF, GPIO_UART_rx_st[UARTx].AFx_en, PP, NoP);	// RX
			// Enable clock for USART peripheral
			SET_BIT(*RCC_APBxENR_UART_arr[UARTx].reg_u32_ptr, RCC_APBxENR_UART_arr[UARTx].pos);
			// Enable Interrupt line
			NVIC_ISER_setVal(UARTx_Interrupt_line[UARTx]);

			is_Init_done_once[UARTx] = true;
		}
		
		// init queue
		queue_init((queue_t*)&UART_recv_buf[UARTx]);
		// Disable USART
		CLEAR_BIT(USART_reg[UARTx]->CR1, 13U);
		// Setting Data layout
		CLEAR_BIT(USART_reg[UARTx]->CR1, 12U);
		// Setting Stop bits
		WRITE_REG(USART_reg[UARTx]->CR2, 3UL, 12U, 0UL);
		// Setting DMA transmitter
		SET_BIT(USART_reg[UARTx]->CR3, 7U);
		// Setting DMA receiver
		SET_BIT(USART_reg[UARTx]->CR3, 6U);
		// Choose oversampling
		CLEAR_BIT(USART_reg[UARTx]->CR1, 15U);
		// Calculate Baudrate
		uint32_t fck = 0;

		if ((USART1 == UARTx) || (USART6 == UARTx))
		{
			fck = APB2_freq;
		}
		else if ((USART2 == UARTx) || (USART3 == UARTx) || (UART4 == UARTx) || (UART5 == UARTx))
		{
			fck = APB1_freq;
		}
		else
		{
			return NOT_OK;
		}

		uint32_t div_mantissa = fck / (8 * (2 - READ_REG(USART_reg[UARTx]->CR1, 1UL, 15U)) * baudrate);
		uint32_t div_fraction_times_100 = ((fck * 100) / (8 * (2 - READ_REG(USART_reg[UARTx]->CR1, 1UL, 15U)) * baudrate)) % 100;
		uint32_t div_fraction = (div_fraction_times_100 * (8 * (2 - READ_REG(USART_reg[UARTx]->CR1, 1UL, 15U))) + 50) / 100;
		uint32_t brr_val = (div_mantissa << 4) | (div_fraction & 0xFU);
		// Setting Baudrate
		WRITE_REG(USART_reg[UARTx]->BRR, 0xFFFFUL, 0U, brr_val);
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
		// LIN mode enable
		CLEAR_BIT(USART_reg[UARTx]->CR2, 14U);
		// Smartcard mode enable
		CLEAR_BIT(USART_reg[UARTx]->CR3, 5U);
		// IrDA mode enable
		CLEAR_BIT(USART_reg[UARTx]->CR3, 1U);
		// Enable/Disable Error interrupt
		SET_BIT(USART_reg[UARTx]->CR3, 0U);
		// Parity error interrupt enable
		CLEAR_BIT(USART_reg[UARTx]->CR1, 8U);
		// IDLE interrupt enable
		SET_BIT(USART_reg[UARTx]->CR1, 4U);
		// TXE interrupt disable
		CLEAR_BIT(USART_reg[UARTx]->CR1, 7U);
		// TC interrupt disable
		CLEAR_BIT(USART_reg[UARTx]->CR1, 6U);
		// RXNE interrupt disable
		CLEAR_BIT(USART_reg[UARTx]->CR1, 5U);
		// Enable USART
		SET_BIT(USART_reg[UARTx]->CR1, 13U);
		// Enable Transmitter
		SET_BIT(USART_reg[UARTx]->CR1, 3U);
		// Enable Receiver
		SET_BIT(USART_reg[UARTx]->CR1, 2U);
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
	if ((NOT_INITTED == UART_init_state[UARTx]) || (UART_STATE_BUSY == UART_state_tx[UARTx]) || (NULL == buf) || (0 == data_length) || (USART1 > UARTx) || (USART6 < UARTx))
	{
		return NOT_OK;
	}
	else
	{
		stream_channel_t Stream_info_st =
		{
			.DMAx = UART_Stream_info_tx_st[UARTx].DMAx,
			.stream = UART_Stream_info_tx_st[UARTx].stream,
			.channel = UART_Stream_info_tx_st[UARTx].channel
		};
		
		UART_state_tx[UARTx] = UART_STATE_BUSY;
		DMA_transfer
		(
			Stream_info_st, 
			(buffer_t){
				.data_length = data_length,
				.peri_addr = &USART_reg[UARTx]->DR,
				.mem_addr = (volatile uint8_t *)buf
			}
		);
	}

	return OK;
}

void UART_Read(UARTx_t UARTx, uint8_t *buf, uint8_t data_length)
{
	if (true == UART_recv_buf[UARTx].isEmpty)
	{
		return;
	}
	else
	{
		uint8_t index = 0;

		// Disable DMA Stream
		CLEAR_BIT(DMA_reg[UART_Stream_info_rx_st[UARTx].DMAx]->S[UART_Stream_info_rx_st[UARTx].stream].CR, 0U);
		while (READ_REG(DMA_reg[UART_Stream_info_rx_st[UARTx].DMAx]->S[UART_Stream_info_rx_st[UARTx].stream].CR, 1UL, 0U));
		// Disable Interrupt
		NVIC_ICER_setVal(UARTx_Interrupt_line[UARTx]);

		if (data_length > ARR_SIZE)
		{
			data_length = ARR_SIZE;
		}

		if (UART_recv_buf[UARTx].rear >= data_length)
		{
			UART_recv_buf[UARTx].front = UART_recv_buf[UARTx].rear - data_length;
		}
		else
		{
			if (UART_recv_buf[UARTx].overrun)
			{
				UART_recv_buf[UARTx].front = ARR_SIZE - data_length + UART_recv_buf[UARTx].rear;
			}
			else
			{
				UART_recv_buf[UARTx].front = 0;
			}
		}

		while (false == UART_recv_buf[UARTx].isEmpty)
		{
			buf[index++] = UART_recv_buf[UARTx].buf[UART_recv_buf[UARTx].front];
			UART_recv_buf[UARTx].front = (UART_recv_buf[UARTx].front + 1) % ARR_SIZE;

			if (UART_recv_buf[UARTx].front == UART_recv_buf[UARTx].rear)
			{
				UART_recv_buf[UARTx].isEmpty = true;
			}
		}

		UART_recv_buf[UARTx].isFull = false;
		UART_recv_buf[UARTx].overrun = false;
		UART_recv_buf[UARTx].front = 0;
		UART_recv_buf[UARTx].rear = 0;
		isUpdated_UART[UARTx] = false;
	    memset((void*)UART_recv_buf[UARTx].buf, 0, ARR_SIZE * sizeof(*UART_recv_buf[UARTx].buf));

		//Enable Stream
		SET_BIT(DMA_reg[UART_Stream_info_rx_st[UARTx].DMAx]->S[UART_Stream_info_rx_st[UARTx].stream].CR, 0U);
		// Enable Interrupt
		NVIC_ISER_setVal(UARTx_Interrupt_line[UARTx]);
	}
}
