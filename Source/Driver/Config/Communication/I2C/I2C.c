#include "I2C.h"
#include "init_static.h"
#include "bit_operator.h"
#include "RCC_header.h"
#include "Interrupt.h"
#include "GPIO.h"
#include "DMA.h"
#include "delay_us.h"

typedef struct
{
	GPIO_ENABLE_t GPIOx_en;
	uint8_t pos_u8;
} GPIOx_posY_t;

static const GPIOx_posY_t GPIO_SCL_st[3] =
{
	{GPIOBEN, 8},
	{GPIOBEN, 10},
	{GPIOAEN, 8}
};

static const GPIOx_posY_t GPIO_SDA_st[3] =
{
	{GPIOBEN, 9},
	{GPIOBEN, 11},
	{GPIOCEN, 9}
};

I2C_t * const I2C_reg[3] = 
{
    (I2C_t *)0x40005400,
    (I2C_t *)0x40005800,
    (I2C_t *)0x40005C00
};

__attribute__((section(".ccmram_data")))volatile bool I2C_init_state[3] =
{
    NOT_INITTED,
    NOT_INITTED,
    NOT_INITTED
};

volatile uint8_t I2C_recv_buf[3][ARR_SIZE] =
{
    {0},
    {0},
    {0}
};

__attribute__((section(".ccmram_data")))volatile bool I2C_state_tx[3] =
{
    I2C_STATE_READY,
    I2C_STATE_READY,
    I2C_STATE_READY
};

__attribute__((section(".ccmram_data")))volatile bool I2C_state_rx[3] =
{
    I2C_STATE_READY,
    I2C_STATE_READY,
    I2C_STATE_READY
};

__attribute__((section(".ccmram_data")))volatile uint8_t isUpdated_I2C[3] =
{
    false,
	false,
	false
};

static const stream_channel_t I2C_Stream_info_tx_st[3] =
{
	// I2C1
	{
		.DMAx = DMA1,
		.stream = Stream_6,	// Stream_7
		.channel = 1
	},
	// I2C2
	{
		.DMAx = DMA1,
		.stream = Stream_7,
		.channel = 7
	},
	// I2C3
	{
		.DMAx = DMA1,
		.stream = Stream_4,
		.channel = 3
	}
};

static const stream_channel_t I2C_Stream_info_rx_st[3] =
{
	// I2C1
	{
		.DMAx = DMA1,
		.stream = Stream_5,	// Stream_0
		.channel = 1
	},
	// I2C2
	{
		.DMAx = DMA1,
		.stream = Stream_3,	// Stream_2
		.channel = 7
	},
	// I2C3
	{
		.DMAx = DMA1,
		.stream = Stream_2,
		.channel = 3
	}
};

static const unsigned int RCC_APBxENR_I2C_pos_arr[3] = {21U, 22U, 23U};

static const peripheral_Selection_t I2Cx_Interrupt_line[3][2] =
{
	{I2C1_EV, I2C1_ER},
	{I2C2_EV, I2C2_ER},
	{I2C3_EV, I2C3_ER}

};

volatile uint8_t I2C_length[3];
volatile uint8_t I2C_addr[3];

static bool I2C_bus_clean(I2Cx_t I2Cx_en);

bool I2C_init(I2Cx_t I2Cx_en)
{
	I2C_init_state[I2Cx_en] = NOT_INITTED;
	static bool is_Init_done_once[3] = {false, false, false};

    if ((I2C_STATE_READY == I2C_state_tx[I2Cx_en]) && (I2C_STATE_READY == I2C_state_rx[I2Cx_en]))
    {
		if (false == is_Init_done_once[I2Cx_en])
		{
        	DMA_direct_param_t DMA_direct_param_tx_st = (DMA_direct_param_t)
			{
				{
					.DMAx = I2C_Stream_info_tx_st[I2Cx_en].DMAx,
					.stream = I2C_Stream_info_tx_st[I2Cx_en].stream,
					.channel = I2C_Stream_info_tx_st[I2Cx_en].channel
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
					.DMAx = I2C_Stream_info_rx_st[I2Cx_en].DMAx,
					.stream = I2C_Stream_info_rx_st[I2Cx_en].stream,
					.channel = I2C_Stream_info_rx_st[I2Cx_en].channel
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
					.dir = peri_to_mem,
					.flow_controller = DMA
				},
				{
					.stream_priority = medium,
					.interrupt_en_u8 = (TCIE | DMEIE)
				}
			};

			// Enable I2C clock
			SET_BIT(RCC_reg->APB1ENR, RCC_APBxENR_I2C_pos_arr[I2Cx_en]);
			// Enable ISR for I2C
        	NVIC_ISER_setVal(I2Cx_Interrupt_line[I2Cx_en][0]);
        	NVIC_ISER_setVal(I2Cx_Interrupt_line[I2Cx_en][1]);

        	// Setting for DMA
			if ((NOT_OK == DMA_direct_init(DMA_direct_param_tx_st)) || (NOT_OK == DMA_direct_init(DMA_direct_param_rx_st)))
			{
				return NOT_OK;
			}

			is_Init_done_once[I2Cx_en] = true;
		}

        // Setup GPIO for I2C
        GPIO_setup(GPIO_SCL_st[I2Cx_en].GPIOx_en, GPIO_SCL_st[I2Cx_en].pos_u8, AF, AF4, OD, NoP);	// SCL
	    GPIO_setup(GPIO_SDA_st[I2Cx_en].GPIOx_en, GPIO_SDA_st[I2Cx_en].pos_u8, AF, AF4, OD, NoP);	// SDA

		// Check Bus busy
		if (READ_REG(I2C_reg[I2Cx_en]->SR2, 1UL, 1U))
		{
			I2C_bus_clean(I2Cx_en);
			return NOT_OK;
		}

		uint32_t I2C_freq_u32 = APB1_freq / 1000000;
		
        // Setting for I2C
        // Peripheral disable
        CLEAR_BIT(I2C_reg[I2Cx_en]->CR1, 0U);
        // Enable Event interrupt enable
        SET_BIT(I2C_reg[I2Cx_en]->CR2, 9U);
        // Enable Error interrupt enable
        SET_BIT(I2C_reg[I2Cx_en]->CR2, 8U);
        // Set Peripheral clock frequency
        WRITE_REG(I2C_reg[I2Cx_en]->CR2, 0x3FUL, 0U, I2C_freq_u32);
        // I2C master mode selection: Sm
        CLEAR_BIT(I2C_reg[I2Cx_en]->CCR, 15U);
        // Clock control register in Fm/Sm mode (Master mode)
        WRITE_REG(I2C_reg[I2Cx_en]->CCR, 0xFFFUL, 0U, (APB1_freq / (2 * 100000U)));
        // Maximum rise time in Fm/Sm mode (Master mode): 1000ns
        WRITE_REG(I2C_reg[I2Cx_en]->TRISE, 0x3FUL, 0U, (I2C_freq_u32 + 1));
        // Peripheral enable
        SET_BIT(I2C_reg[I2Cx_en]->CR1, 0U);
		I2C_init_state[I2Cx_en] = INITTED;
    }
    else
    {
        return NOT_OK;
    }

    return OK;
}

bool I2C_master_send(I2Cx_t I2Cx_en, uint8_t addr_u8, const uint8_t *buf, uint32_t length_u32)
{
	if ((NULL == buf) || (0 == length_u32))
	{
		return NOT_OK;
	}
	else if (NOT_INITTED == I2C_init_state[I2Cx_en])
	{
		if (NOT_OK == I2C_init(I2Cx_en))
		{
			return NOT_OK;
		}
	}
	else
	{
		// do nothing
	}

	// Check Bus busy
	if (
		(I2C_STATE_BUSY == I2C_state_tx[I2Cx_en])
		|| (I2C_STATE_BUSY == I2C_state_rx[I2Cx_en])
		|| READ_REG(I2C_reg[I2Cx_en]->SR2, 1UL, 1U)
	)
	{
		uint32_t timeStart_u32 = SysTick_cnt_u32;

		// Wait for Ongoing communication done
		while (READ_REG(I2C_reg[I2Cx_en]->SR2, 1UL, 1U))
		{
			// Timeout is between 10ms and 20ms
			if (2UL <= (SysTick_cnt_u32 - timeStart_u32))
			{
				if (NOT_OK == I2C_bus_clean(I2Cx_en))
				{
					return NOT_OK;
				}
			}
		}
	}

	buffer_t buffer_info_tx_st = (buffer_t)
	{
		.data_length = length_u32,
		.peri_addr = &I2C_reg[I2Cx_en]->DR,
		.mem_addr = (volatile uint32_t *)buf
	};

	stream_channel_t Stream_info_st =
	{
		.DMAx = I2C_Stream_info_tx_st[I2Cx_en].DMAx,
		.stream = I2C_Stream_info_tx_st[I2Cx_en].stream,
		.channel = I2C_Stream_info_tx_st[I2Cx_en].channel
	};

	I2C_state_tx[I2Cx_en] = I2C_STATE_BUSY;
	// Write address: LSB = 0 for transmitter
	I2C_addr[I2Cx_en] = (addr_u8 << 1U);
	// Setup DMA
	DMA_transfer(Stream_info_st, buffer_info_tx_st);
	// Ensure ACK is enabled (though primarily needed for RX, it's good practice)
	SET_BIT(I2C_reg[I2Cx_en]->CR1, 10U);
	// DMA requests enable
    SET_BIT(I2C_reg[I2Cx_en]->CR2, 11U);
    // Generate START Condition
	SET_BIT(I2C_reg[I2Cx_en]->CR1, 8U);

	return OK;
}

bool I2C_master_recv(I2Cx_t I2Cx_en, uint8_t addr_u8, uint32_t length_u32)
{
	if (length_u32 > sizeof(I2C_recv_buf[I2Cx_en]))
	{
		return NOT_OK;
	}
	else if (NOT_INITTED == I2C_init_state[I2Cx_en])
	{
		if (NOT_OK == I2C_init(I2Cx_en))
		{
			return NOT_OK;
		}
	}
	else
	{
		// do nothing
	}

	// Check Bus busy
	if (
		(I2C_STATE_BUSY == I2C_state_tx[I2Cx_en])
		|| (I2C_STATE_BUSY == I2C_state_rx[I2Cx_en])
		|| READ_REG(I2C_reg[I2Cx_en]->SR2, 1UL, 1U)
	)
	{
		uint32_t timeStart_u32 = SysTick_cnt_u32;

		// Wait for Ongoing communication done
		while (READ_REG(I2C_reg[I2Cx_en]->SR2, 1UL, 1U))
		{
			// Timeout is between 10ms and 20ms
			if (2UL <= (SysTick_cnt_u32 - timeStart_u32))
			{
				if (NOT_OK == I2C_bus_clean(I2Cx_en))
				{
					return NOT_OK;
				}
			}
		}
	}

	buffer_t buffer_info_rx_st = (buffer_t)
	{
		.data_length = length_u32 - 2,
		.peri_addr = &I2C_reg[I2Cx_en]->DR,
		.mem_addr = (volatile uint32_t *)I2C_recv_buf[I2Cx_en]
	};

	stream_channel_t Stream_info_st =
	{
		.DMAx = I2C_Stream_info_rx_st[I2Cx_en].DMAx,
		.stream = I2C_Stream_info_rx_st[I2Cx_en].stream,
		.channel = I2C_Stream_info_rx_st[I2Cx_en].channel
	};

	isUpdated_I2C[I2Cx_en] = false;
	I2C_state_rx[I2Cx_en] = I2C_STATE_BUSY;
	I2C_length[I2Cx_en] = length_u32;
	// Clear Buffer interrupt enable
	CLEAR_BIT(I2C_reg[I2Cx_en]->CR2, 10U);
	// Reset buffer BEFORE starting DMA to avoid race conditions
	for (unsigned int i = 0; i < sizeof(I2C_recv_buf[I2Cx_en]); i++)
    {
        I2C_recv_buf[I2Cx_en][i] = 0;
    }
	// Write address: LSB = 1 for receiver
	I2C_addr[I2Cx_en] = ((addr_u8 << 1U) | 1U);
	// Setup DMA
	DMA_transfer(Stream_info_st, buffer_info_rx_st);
	// Ensure ACK is enabled before starting
	SET_BIT(I2C_reg[I2Cx_en]->CR1, 10U);
	// DMA requests enable
    SET_BIT(I2C_reg[I2Cx_en]->CR2, 11U);
    // Generate START Condition
	SET_BIT(I2C_reg[I2Cx_en]->CR1, 8U);

	return OK;
}

static bool I2C_bus_clean(I2Cx_t I2Cx_en)
{
	if (I2Cx_en > I2C3)
	{
		// do nothing
	}
	else
	{
		uint32_t timeStart_u32 = SysTick_cnt_u32;

		I2C_state_tx[I2Cx_en] = I2C_STATE_READY;
		I2C_state_rx[I2Cx_en] = I2C_STATE_READY;

		// Wait for Bus ready (BUSY)
		while (READ_REG(I2C_reg[I2Cx_en]->SR2, 1UL, 1U))
		{
			// Timeout is between 10ms and 20ms
			if (2UL >= (SysTick_cnt_u32 - timeStart_u32))
			{
				// Peripheral disable
        		CLEAR_BIT(I2C_reg[I2Cx_en]->CR1, 0U);
				// Setup PIN SCL and SDA as GP mode
    	    	GPIO_setup(GPIO_SCL_st[I2Cx_en].GPIOx_en, GPIO_SCL_st[I2Cx_en].pos_u8, GP_OUT, AF4, OD, NoP);	// SCL
		    	GPIO_setup(GPIO_SDA_st[I2Cx_en].GPIOx_en, GPIO_SDA_st[I2Cx_en].pos_u8, IN, AF4, OD, NoP);	// SDA
			
				for (uint8_t i = 0; i < 9; i++)
				{
					GPIO_OUT_setVal(GPIO_SCL_st[I2Cx_en].GPIOx_en, GPIO_SCL_st[I2Cx_en].pos_u8, 0);
					delay_us(5);
    				GPIO_OUT_setVal(GPIO_SCL_st[I2Cx_en].GPIOx_en, GPIO_SCL_st[I2Cx_en].pos_u8, 1);
					delay_us(5);

					// Check Bus ready
					if (1 == GPIO_IN_getVal(GPIO_SDA_st[I2Cx_en].GPIOx_en, GPIO_SDA_st[I2Cx_en].pos_u8))
					{
						break;
					}
				}

				// Generate Stop condition manually
				GPIO_setup(GPIO_SDA_st[I2Cx_en].GPIOx_en, GPIO_SDA_st[I2Cx_en].pos_u8, GP_OUT, AF4, OD, NoP);	// SDA
				GPIO_OUT_setVal(GPIO_SDA_st[I2Cx_en].GPIOx_en, GPIO_SDA_st[I2Cx_en].pos_u8, 0);
				delay_us(5);
    			GPIO_OUT_setVal(GPIO_SCL_st[I2Cx_en].GPIOx_en, GPIO_SCL_st[I2Cx_en].pos_u8, 1);
				delay_us(5);
				GPIO_OUT_setVal(GPIO_SDA_st[I2Cx_en].GPIOx_en, GPIO_SDA_st[I2Cx_en].pos_u8, 1);

				// Clear fail status
				// Software reset
				SET_BIT(I2C_reg[I2Cx_en]->CR1, 15U);
				delay_us(5);
				// clear reset bit
				CLEAR_BIT(I2C_reg[I2Cx_en]->CR1, 15U);

        		// Setup GPIO for I2C
        		GPIO_setup(GPIO_SCL_st[I2Cx_en].GPIOx_en, GPIO_SCL_st[I2Cx_en].pos_u8, AF, AF4, OD, NoP);	// SCL
	    		GPIO_setup(GPIO_SDA_st[I2Cx_en].GPIOx_en, GPIO_SDA_st[I2Cx_en].pos_u8, AF, AF4, OD, NoP);	// SDA

				// Peripheral enable
        		SET_BIT(I2C_reg[I2Cx_en]->CR1, 0U);

				if (READ_REG(I2C_reg[I2Cx_en]->SR2, 1UL, 1U))
				{
					return OK;
				}
			}
			else
			{
				break;
			}
		}
	}

	return NOT_OK;
}
