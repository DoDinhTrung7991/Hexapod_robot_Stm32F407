#include "SPI.h"

typedef struct
{
	volatile uint32_t *reg_u32_ptr;
	unsigned int pos;
} RCC_APBxENR_SPI_t;

SPI_t *SPI[3] = 
{
    (SPI_t *)0x40005400,
    (SPI_t *)0x40005800,
    (SPI_t *)0x40005C00
};

static GPIO_SPI_t GPIO_SPI_MISO_st[3] =
{
    {GPIOAEN, 6U}, // {GPIOBEN, 4U}
    {GPIOCEN, 2U}, // {GPIOBEN, 14U}
    {GPIOCEN, 11U} // {GPIOBEN, 4U}
};

static GPIO_SPI_t GPIO_SPI_MOSI_st[3] =
{
    {GPIOAEN, 7U}, // {GPIOBEN, 5U}
    {GPIOCEN, 3U}, // {GPIOBEN, 15U}
    {GPIOCEN, 12U} // {GPIOBEN, 5U}
};

static GPIO_SPI_t GPIO_SPI_NSS_st[3] =
{
    {GPIOAEN, 4U}, // {GPIOAEN, 15U}
    {GPIOBEN, 12U}, // {GPIOBEN, 9U}
    {GPIOAEN, 4U} // {GPIOAEN, 15U}
};

static GPIO_SPI_t GPIO_SPI_SCK_st[3] =
{
    {GPIOAEN, 5U}, // {GPIOBEN, 3U}
    {GPIOBEN, 10U}, // {GPIOBEN, 13U}
    {GPIOCEN, 10U} // {GPIOBEN, 3U}
};

static AFRx_t SPI_GPIO_AFx[3] =
{
    AF5,
    AF5,
    AF6
};

static bool SPI_init_state[3] = 
{
    NOT_INITTED,
    NOT_INITTED,
    NOT_INITTED
};

static const stream_channel_t SPI_Stream_info_tx_st[3] =
{
	// SPI1
	{
		.DMAx = DMA2,
		.stream = Stream_3,
		.channel = 3
	},
	// SPI2
	{
		.DMAx = DMA1,
		.stream = Stream_4,
		.channel = 0
	},
	// SPI3
	{
		.DMAx = DMA1,
		.stream = Stream_7,
		.channel = 0
	}
};

static const stream_channel_t SPI_Stream_info_rx_st[3] =
{
	// SPI1
	{
		.DMAx = DMA2,
		.stream = Stream_0,
		.channel = 3
	},
	// SPI2
	{
		.DMAx = DMA1,
		.stream = Stream_3,
		.channel = 0
	},
	// SPI3
	{
		.DMAx = DMA1,
		.stream = Stream_2,
		.channel = 0
	}
};

static const unsigned int SPIx_Interrupt_line[3] =
{
	SPI1_Interrupt,
	SPI2_Interrupt,
	SPI3_Interrupt
};

static uint8_t SPI_recv_buf[3][ARR_SIZE];

static RCC_APBxENR_SPI_t RCC_APBxENR_SPI_arr[3] =
{
	{&RCC_reg->APB2ENR, 12U},
	{&RCC_reg->APB1ENR, 14U},
	{&RCC_reg->APB1ENR, 15U}
};

bool SPI_init(SPIx_t SPIx_en, SPI_mode_t mode_en, SPI_sampling_mode_t SPI_sampling_mode_en, SPI_baudrate_t baudrate_t)
{
    static bool is_Init_done_once[3] = {false, false, false};
	// Reset Init flag
    SPI_init_state[SPIx_en] = NOT_INITTED;
	
    // Check if BUSY
    if ((READ_REG(SPI[SPIx_en]->SR, 1UL, 1U) && (!READ_REG(SPI[SPIx_en]->SR, 1UL, 0U))) && (!READ_REG(SPI[SPIx_en]->SR, 1UL, 7U)))
    {
		if (!is_Init_done_once[SPIx_en])
		{
			DMA_direct_param_t DMA_direct_param_tx_st = (DMA_direct_param_t)
			{
				{
					.DMAx = SPI_Stream_info_tx_st[SPIx_en].DMAx,
					.stream = SPI_Stream_info_tx_st[SPIx_en].stream,
					.channel = SPI_Stream_info_tx_st[SPIx_en].channel
				},
				{
					.double_buffer_en = disable,
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
					.DMAx = SPI_Stream_info_rx_st[SPIx_en].DMAx,
					.stream = SPI_Stream_info_rx_st[SPIx_en].stream,
					.channel = SPI_Stream_info_rx_st[SPIx_en].channel
				},
				{
					.double_buffer_en = disable,
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
		
    		// Setting for DMA
			if ((NOT_OK == DMA_direct_init(DMA_direct_param_tx_st)) || (NOT_OK == DMA_direct_init(DMA_direct_param_rx_st)))
			{
				return NOT_OK;
			}

			// Enable clock
			SET_BIT(*RCC_APBxENR_SPI_arr[SPIx_en].reg_u32_ptr, RCC_APBxENR_SPI_arr[SPIx_en].pos);

			// Enable Interrupt line
    		NVIC_ISER_setVal(SPIx_Interrupt_line[SPIx_en]);

			is_Init_done_once[SPIx_en] = true;
		}

        // Disable SPI
        CLEAR_BIT(SPI[SPIx_en]->CR1, 6U); // SPE bit
        // Set sampling mode
        WRITE_REG(SPI[SPIx_en]->CR1, 3UL, 0U, SPI_sampling_mode_en); // Write CPOL and CPHA
        // Setup Data frame format
        CLEAR_BIT(SPI[SPIx_en]->CR1, 11U); // 8-bit length - DFF register
        // Setup Frame format
        CLEAR_BIT(SPI[SPIx_en]->CR1, 7U); // MSB transmitted first - LSBFIRST register
        // Disable Software NSS pin management
        CLEAR_BIT(SPI[SPIx_en]->CR1, 9U); // SSM bit

        if (master == mode_en)
        {
            // Set Baudrate
            WRITE_REG(SPI[SPIx_en]->CR1, 7UL, 3U, baudrate_t); // Write BR
            // Set SS output enable
            SET_BIT(SPI[SPIx_en]->CR2, 2U); // SS output is enabled in master mode
            // Master selection
            SET_BIT(SPI[SPIx_en]->CR1, 2U); // Master configuration
        }
        else
        {
            // Clear SS output enable
            CLEAR_BIT(SPI[SPIx_en]->CR2, 2U); // SS output is disabled in slave mode
            // Slave selection
            CLEAR_BIT(SPI[SPIx_en]->CR1, 2U); // Master configuration
        }

        // Setup Frame format
        CLEAR_BIT(SPI[SPIx_en]->CR2, 4U); // FRF bit

		// Enable interrupt
		// Enable Error interrupt
		SET_BIT(SPI[SPIx_en]->CR2, 5U); // ERRIE bit

        //Setup GPIO
        GPIO_setup(GPIO_SPI_MISO_st[SPIx_en].GPIOx_en, GPIO_SPI_MISO_st[SPIx_en].pos_u8, AF, SPI_GPIO_AFx[SPIx_en], PP, NoP);
        GPIO_setup(GPIO_SPI_MOSI_st[SPIx_en].GPIOx_en, GPIO_SPI_MOSI_st[SPIx_en].pos_u8, AF, SPI_GPIO_AFx[SPIx_en], PP, NoP);
        GPIO_setup(GPIO_SPI_NSS_st[SPIx_en].GPIOx_en, GPIO_SPI_NSS_st[SPIx_en].pos_u8, AF, SPI_GPIO_AFx[SPIx_en], PP, PU);
        GPIO_setup(GPIO_SPI_SCK_st[SPIx_en].GPIOx_en, GPIO_SPI_SCK_st[SPIx_en].pos_u8, AF, SPI_GPIO_AFx[SPIx_en], PP, NoP);

        // Set Init flag
        SPI_init_state[SPIx_en] = INITTED;
    }
    else
    {
        return NOT_OK;
    }

    return OK;
}

bool SPI_send_receive(SPIx_t SPIx_en, const uint8_t *str, uint32_t length_u32)
{
	// Check if BUSY
	if (
		(
			READ_REG(SPI[SPIx_en]->SR, 1UL, 1U) 
			&& (!READ_REG(SPI[SPIx_en]->SR, 1UL, 0U))
		)
		&& (!READ_REG(SPI[SPIx_en]->SR, 1UL, 7U))
		&& (INITTED == SPI_init_state[SPIx_en])
	)
	{
		buffer_t buffer_info_tx_st = (buffer_t)
		{
			.data_length = length_u32,
			.peri_addr = &SPI[SPIx_en]->DR,
			.mem_addr = (volatile uint8_t *)str
		};

		stream_channel_t Stream_info_tx_st =
		{
			.DMAx = SPI_Stream_info_tx_st[SPIx_en].DMAx,
			.stream = SPI_Stream_info_tx_st[SPIx_en].stream,
			.channel = SPI_Stream_info_tx_st[SPIx_en].channel
		};

		buffer_t buffer_info_rx_st = (buffer_t)
		{
			.data_length = length_u32,
			.peri_addr = &SPI[SPIx_en]->DR,
			.mem_addr = (volatile uint8_t *)SPI_recv_buf[SPIx_en]
		};

		stream_channel_t Stream_info_rx_st =
		{
			.DMAx = SPI_Stream_info_rx_st[SPIx_en].DMAx,
			.stream = SPI_Stream_info_rx_st[SPIx_en].stream,
			.channel = SPI_Stream_info_rx_st[SPIx_en].channel
		};

		// Setup DMA tx
		DMA_transfer(Stream_info_tx_st, buffer_info_tx_st);
		// Setup DMA rx
		DMA_transfer(Stream_info_rx_st, buffer_info_rx_st);
		// Enable DMA
		SET_BIT(SPI[SPIx_en]->CR2, 0U); // RXDMAEN bit
		SET_BIT(SPI[SPIx_en]->CR2, 1U); // TXDMAEN bit
		// Enable SPI
        SET_BIT(SPI[SPIx_en]->CR1, 6U); // SPE bit
	}
	else
	{
		return NOT_OK;
	}

	return OK;
}
