#include "DMA.h"

#define INTERRUPT_CLEARMASK 0x3DUL

typedef enum
{
	interruptFlag_0or4 = 0,
	interruptFlag_1or5 = 6,
	interruptFlag_2or6 = 16,
	interruptFlag_3or7 = 22
} clearFlag_shiftBit_t;

const uint8_t clearFlag_shiftBit_arr[4] =
{
	interruptFlag_0or4,
	interruptFlag_1or5,
	interruptFlag_2or6,
	interruptFlag_3or7
};

DMA_t *DMA_reg[2] =
{
	(DMA_t*)0x40026000,
	(DMA_t*)0x40026400
};

static unsigned int RCC_APBxENR_DMA_pos_arr[2] = {21U, 22U};

static void Enable_DMA_interruptLine(stream_channel_t Stream_info_st);

bool DMA_direct_init(DMA_direct_param_t DMA_direct_param_st)
{
    uint32_t timeStart_u32;

	if ((7 < DMA_direct_param_st.Stream_info_st.channel) || (Stream_7 < DMA_direct_param_st.Stream_info_st.stream))
	{
		return NOT_OK;
	}

	// Enable Clock
	SET_BIT(RCC_reg->AHB1ENR, RCC_APBxENR_DMA_pos_arr[DMA_direct_param_st.Stream_info_st.DMAx]);

	//Reset DMA_SxCR
	DMA_reg[DMA_direct_param_st.Stream_info_st.DMAx]->S[DMA_direct_param_st.Stream_info_st.stream].CR = 0UL;
	timeStart_u32 = SysTick_cnt_u32;

	while (READ_REG(DMA_reg[DMA_direct_param_st.Stream_info_st.DMAx]->S[DMA_direct_param_st.Stream_info_st.stream].CR, 1UL, 0U))
	{
		if (
			((SysTick_cnt_u32 < timeStart_u32) && (5UL <= (SysTick_cnt_u32 + (0xFFFFFFFFUL - timeStart_u32) + 1)))
			|| ((SysTick_cnt_u32 >= timeStart_u32) && (5UL <= (SysTick_cnt_u32 - timeStart_u32)))
		)
		{
			return NOT_OK;
		}
	}

	// Direct mode enable
	CLEAR_BIT(DMA_reg[DMA_direct_param_st.Stream_info_st.DMAx]->S[DMA_direct_param_st.Stream_info_st.stream].FCR, 2U);
	// Channel selection
	WRITE_REG(DMA_reg[DMA_direct_param_st.Stream_info_st.DMAx]->S[DMA_direct_param_st.Stream_info_st.stream].CR, 7UL, 25U, DMA_direct_param_st.Stream_info_st.channel);

	// Double buffer mode
	if (enable == DMA_direct_param_st.data_info_st.double_buffer_en)
	{
		SET_BIT(DMA_reg[DMA_direct_param_st.Stream_info_st.DMAx]->S[DMA_direct_param_st.Stream_info_st.stream].CR, 18U);
	}
	else
	{
		CLEAR_BIT(DMA_reg[DMA_direct_param_st.Stream_info_st.DMAx]->S[DMA_direct_param_st.Stream_info_st.stream].CR, 18U);
	}

	// Priority level
	WRITE_REG(DMA_reg[DMA_direct_param_st.Stream_info_st.DMAx]->S[DMA_direct_param_st.Stream_info_st.stream].CR, 3UL, 16U, DMA_direct_param_st.priority_interrupt_st.stream_priority);
	// Memory data size
	WRITE_REG(DMA_reg[DMA_direct_param_st.Stream_info_st.DMAx]->S[DMA_direct_param_st.Stream_info_st.stream].CR, 3UL, 13U, DMA_direct_param_st.data_info_st.mem_data_size);
	// Peripheral data size
	WRITE_REG(DMA_reg[DMA_direct_param_st.Stream_info_st.DMAx]->S[DMA_direct_param_st.Stream_info_st.stream].CR, 3UL, 11U, DMA_direct_param_st.data_info_st.peri_data_size);

	// Memory increment mode
	switch (DMA_direct_param_st.data_info_st.mem_mode)
	{
		case fixed:
			CLEAR_BIT(DMA_reg[DMA_direct_param_st.Stream_info_st.DMAx]->S[DMA_direct_param_st.Stream_info_st.stream].CR, 10U);
			break;

		case not_fixed:
			SET_BIT(DMA_reg[DMA_direct_param_st.Stream_info_st.DMAx]->S[DMA_direct_param_st.Stream_info_st.stream].CR, 10U);
			break;
		
		case circular:
			SET_BIT(DMA_reg[DMA_direct_param_st.Stream_info_st.DMAx]->S[DMA_direct_param_st.Stream_info_st.stream].CR, 10U);
			// Circular mode
			SET_BIT(DMA_reg[DMA_direct_param_st.Stream_info_st.DMAx]->S[DMA_direct_param_st.Stream_info_st.stream].CR, 8U);
			break;

		default:
			return NOT_OK;
			break;
	}
	
	// Peripheral increment mode
	switch (DMA_direct_param_st.data_info_st.peri_mode)
	{
		case fixed:
			CLEAR_BIT(DMA_reg[DMA_direct_param_st.Stream_info_st.DMAx]->S[DMA_direct_param_st.Stream_info_st.stream].CR, 9U);
			break;

		case not_fixed:
			SET_BIT(DMA_reg[DMA_direct_param_st.Stream_info_st.DMAx]->S[DMA_direct_param_st.Stream_info_st.stream].CR, 9U);
			break;
		
		case circular:
			SET_BIT(DMA_reg[DMA_direct_param_st.Stream_info_st.DMAx]->S[DMA_direct_param_st.Stream_info_st.stream].CR, 9U);
			// Circular mode
			SET_BIT(DMA_reg[DMA_direct_param_st.Stream_info_st.DMAx]->S[DMA_direct_param_st.Stream_info_st.stream].CR, 8U);
			break;

		default:
			return NOT_OK;
			break;
	}
	
	// Data transfer direction
	WRITE_REG(DMA_reg[DMA_direct_param_st.Stream_info_st.DMAx]->S[DMA_direct_param_st.Stream_info_st.stream].CR, 3UL, 6U, DMA_direct_param_st.dir_control_st.dir);

	// Peripheral flow controller
	if (DMA == DMA_direct_param_st.dir_control_st.flow_controller)
	{
		CLEAR_BIT(DMA_reg[DMA_direct_param_st.Stream_info_st.DMAx]->S[DMA_direct_param_st.Stream_info_st.stream].CR, 5U);
	}
	else if (PERIPHERAL == DMA_direct_param_st.dir_control_st.flow_controller)
	{
		SET_BIT(DMA_reg[DMA_direct_param_st.Stream_info_st.DMAx]->S[DMA_direct_param_st.Stream_info_st.stream].CR, 5U);
	}
	else
	{
		return NOT_OK;
	}

	// Transfer complete interrupt enable
	// Half transfer interrupt enable
	// Transfer error interrupt enable
	// Direct mode error interrupt enable
	WRITE_REG(DMA_reg[DMA_direct_param_st.Stream_info_st.DMAx]->S[DMA_direct_param_st.Stream_info_st.stream].CR, 0xFUL, 1U, DMA_direct_param_st.priority_interrupt_st.interrupt_en_u8);

	if (DMA_direct_param_st.priority_interrupt_st.interrupt_en_u8)
	{
		Enable_DMA_interruptLine(DMA_direct_param_st.Stream_info_st);
	}

	return OK;
}

bool DMA_FIFO_init(DMA_FIFO_param_t DMA_FIFO_param_st)
{
    uint32_t timeStart_u32;

	if ((7 < DMA_FIFO_param_st.Stream_info_st.channel) || (Stream_7 < DMA_FIFO_param_st.Stream_info_st.stream))
	{
		return NOT_OK;
	}

	// Enable Clock
	SET_BIT(RCC_reg->AHB1ENR, RCC_APBxENR_DMA_pos_arr[DMA_FIFO_param_st.Stream_info_st.DMAx]);

	//Reset DMA_SxCR
	DMA_reg[DMA_FIFO_param_st.Stream_info_st.DMAx]->S[DMA_FIFO_param_st.Stream_info_st.stream].CR = 0UL;
	timeStart_u32 = SysTick_cnt_u32;

	while (READ_REG(DMA_reg[DMA_FIFO_param_st.Stream_info_st.DMAx]->S[DMA_FIFO_param_st.Stream_info_st.stream].CR, 1UL, 0U))
	{
		if (
			((SysTick_cnt_u32 < timeStart_u32) && (5UL <= (SysTick_cnt_u32 + (0xFFFFFFFFUL - timeStart_u32) + 1)))
			|| ((SysTick_cnt_u32 >= timeStart_u32) && (5UL <= (SysTick_cnt_u32 - timeStart_u32)))
		)
		{
			return NOT_OK;
		}
	}

	// FIFO mode enable
	SET_BIT(DMA_reg[DMA_FIFO_param_st.Stream_info_st.DMAx]->S[DMA_FIFO_param_st.Stream_info_st.stream].FCR, 2U);
	// Channel selection
	WRITE_REG(DMA_reg[DMA_FIFO_param_st.Stream_info_st.DMAx]->S[DMA_FIFO_param_st.Stream_info_st.stream].CR, 7UL, 25U, DMA_FIFO_param_st.Stream_info_st.channel);

	// Double buffer mode
	if (enable == DMA_FIFO_param_st.data_info_st.double_buffer_en)
	{
		SET_BIT(DMA_reg[DMA_FIFO_param_st.Stream_info_st.DMAx]->S[DMA_FIFO_param_st.Stream_info_st.stream].CR, 18U);
	}
	else
	{
		CLEAR_BIT(DMA_reg[DMA_FIFO_param_st.Stream_info_st.DMAx]->S[DMA_FIFO_param_st.Stream_info_st.stream].CR, 18U);
	}

	// Priority level
	WRITE_REG(DMA_reg[DMA_FIFO_param_st.Stream_info_st.DMAx]->S[DMA_FIFO_param_st.Stream_info_st.stream].CR, 3UL, 16U, DMA_FIFO_param_st.priority_interrupt_st.stream_priority);
	// Memory burst transfer configuration
	WRITE_REG(DMA_reg[DMA_FIFO_param_st.Stream_info_st.DMAx]->S[DMA_FIFO_param_st.Stream_info_st.stream].CR, 3UL, 23U, DMA_FIFO_param_st.data_info_st.MBURST);
	// Peripheral burst transfer configuration
	WRITE_REG(DMA_reg[DMA_FIFO_param_st.Stream_info_st.DMAx]->S[DMA_FIFO_param_st.Stream_info_st.stream].CR, 3UL, 21U, DMA_FIFO_param_st.data_info_st.PBURST);

	// Peripheral increment offset size
	if (fix_32bit == DMA_FIFO_param_st.data_info_st.peri_inc_offset_en)
	{
		SET_BIT(DMA_reg[DMA_FIFO_param_st.Stream_info_st.DMAx]->S[DMA_FIFO_param_st.Stream_info_st.stream].CR, 15U);
	}
	else
	{
		CLEAR_BIT(DMA_reg[DMA_FIFO_param_st.Stream_info_st.DMAx]->S[DMA_FIFO_param_st.Stream_info_st.stream].CR, 15U);
	}

	// Memory data size
	WRITE_REG(DMA_reg[DMA_FIFO_param_st.Stream_info_st.DMAx]->S[DMA_FIFO_param_st.Stream_info_st.stream].CR, 3UL, 13U, DMA_FIFO_param_st.data_info_st.mem_data_size);
	// Peripheral data size
	WRITE_REG(DMA_reg[DMA_FIFO_param_st.Stream_info_st.DMAx]->S[DMA_FIFO_param_st.Stream_info_st.stream].CR, 3UL, 11U, DMA_FIFO_param_st.data_info_st.peri_data_size);

	// Memory increment mode
	switch (DMA_FIFO_param_st.data_info_st.mem_mode)
	{
		case fixed:
			CLEAR_BIT(DMA_reg[DMA_FIFO_param_st.Stream_info_st.DMAx]->S[DMA_FIFO_param_st.Stream_info_st.stream].CR, 10U);
			break;

		case not_fixed:
			SET_BIT(DMA_reg[DMA_FIFO_param_st.Stream_info_st.DMAx]->S[DMA_FIFO_param_st.Stream_info_st.stream].CR, 10U);
			break;
		
		case circular:
			SET_BIT(DMA_reg[DMA_FIFO_param_st.Stream_info_st.DMAx]->S[DMA_FIFO_param_st.Stream_info_st.stream].CR, 10U);
			// Circular mode
			SET_BIT(DMA_reg[DMA_FIFO_param_st.Stream_info_st.DMAx]->S[DMA_FIFO_param_st.Stream_info_st.stream].CR, 8U);
			break;

		default:
			return NOT_OK;
			break;
	}
	
	// Peripheral increment mode
	switch (DMA_FIFO_param_st.data_info_st.peri_mode)
	{
		case fixed:
			CLEAR_BIT(DMA_reg[DMA_FIFO_param_st.Stream_info_st.DMAx]->S[DMA_FIFO_param_st.Stream_info_st.stream].CR, 9U);
			break;

		case not_fixed:
			SET_BIT(DMA_reg[DMA_FIFO_param_st.Stream_info_st.DMAx]->S[DMA_FIFO_param_st.Stream_info_st.stream].CR, 9U);
			break;
		
		case circular:
			SET_BIT(DMA_reg[DMA_FIFO_param_st.Stream_info_st.DMAx]->S[DMA_FIFO_param_st.Stream_info_st.stream].CR, 9U);
			// Circular mode
			SET_BIT(DMA_reg[DMA_FIFO_param_st.Stream_info_st.DMAx]->S[DMA_FIFO_param_st.Stream_info_st.stream].CR, 8U);
			break;

		default:
			return NOT_OK;
			break;
	}
	
	// Data transfer direction
	WRITE_REG(DMA_reg[DMA_FIFO_param_st.Stream_info_st.DMAx]->S[DMA_FIFO_param_st.Stream_info_st.stream].CR, 3UL, 6U, DMA_FIFO_param_st.dir_control_st.dir);

	// Peripheral flow controller
	if (DMA == DMA_FIFO_param_st.dir_control_st.flow_controller)
	{
		CLEAR_BIT(DMA_reg[DMA_FIFO_param_st.Stream_info_st.DMAx]->S[DMA_FIFO_param_st.Stream_info_st.stream].CR, 5U);
	}
	else if (PERIPHERAL == DMA_FIFO_param_st.dir_control_st.flow_controller)
	{
		SET_BIT(DMA_reg[DMA_FIFO_param_st.Stream_info_st.DMAx]->S[DMA_FIFO_param_st.Stream_info_st.stream].CR, 5U);
	}
	else
	{
		return NOT_OK;
	}

	// Transfer complete interrupt enable
	// Half transfer interrupt enable
	// Transfer error interrupt enable
	// Direct mode error interrupt enable
	WRITE_REG(DMA_reg[DMA_FIFO_param_st.Stream_info_st.DMAx]->S[DMA_FIFO_param_st.Stream_info_st.stream].CR, 0xFUL, 1U, DMA_FIFO_param_st.priority_interrupt_st.interrupt_en_u8);

	if (DMA_FIFO_param_st.priority_interrupt_st.interrupt_en_u8)
	{
		Enable_DMA_interruptLine(DMA_FIFO_param_st.Stream_info_st);
	}

	WRITE_REG(DMA_reg[DMA_FIFO_param_st.Stream_info_st.DMAx]->S[DMA_FIFO_param_st.Stream_info_st.stream].FCR, 3UL, 0U, DMA_FIFO_param_st.FTH_en);

	return OK;
}

void DMA_transfer(stream_channel_t Stream_info_st, buffer_t buffer_info_st)
{
	uint32_t timeStart_u32;

	// Disable DMA Stream
	CLEAR_BIT(DMA_reg[Stream_info_st.DMAx]->S[Stream_info_st.stream].CR, 0U);
	timeStart_u32 = SysTick_cnt_u32;

	while (READ_REG(DMA_reg[Stream_info_st.DMAx]->S[Stream_info_st.stream].CR, 1UL, 0U))
	{
		if (
			((SysTick_cnt_u32 < timeStart_u32) && (5UL <= (SysTick_cnt_u32 + (0xFFFFFFFFUL - timeStart_u32) + 1)))
			|| ((SysTick_cnt_u32 >= timeStart_u32) && (5UL <= (SysTick_cnt_u32 - timeStart_u32)))
		)
		{
			return;
		}
	}

	DMA_reg[Stream_info_st.DMAx]->S[Stream_info_st.stream].NDTR = (uint32_t)buffer_info_st.data_length;
	DMA_reg[Stream_info_st.DMAx]->S[Stream_info_st.stream].PAR = (uint32_t)buffer_info_st.peri_addr;
	DMA_reg[Stream_info_st.DMAx]->S[Stream_info_st.stream].MAR[0] = (uint32_t)buffer_info_st.mem_addr;
	// Enable DMA Stream
	SET_BIT(DMA_reg[Stream_info_st.DMAx]->S[Stream_info_st.stream].CR, 0U);
}

static void Enable_DMA_interruptLine(stream_channel_t Stream_info_st)
{
	uint8_t pos_u8 = Stream_info_st.stream / 4;
	uint8_t clearPos_u8 = Stream_info_st.stream % 4;

	WRITE_REG(DMA_reg[Stream_info_st.DMAx]->IFCR[pos_u8], INTERRUPT_CLEARMASK, clearFlag_shiftBit_arr[clearPos_u8], INTERRUPT_CLEARMASK);

	if (DMA1 == Stream_info_st.DMAx)
	{
		switch (Stream_info_st.stream)
		{
			case Stream_0:
				NVIC_ISER_setVal(DMA1_Stream0);
				break;

			case Stream_1:
				NVIC_ISER_setVal(DMA1_Stream1);
				break;

			case Stream_2:
				NVIC_ISER_setVal(DMA1_Stream2);
				break;

			case Stream_3:
				NVIC_ISER_setVal(DMA1_Stream3);
				break;

			case Stream_4:
				NVIC_ISER_setVal(DMA1_Stream4);
				break;

			case Stream_5:
				NVIC_ISER_setVal(DMA1_Stream5);
				break;

			case Stream_6:
				NVIC_ISER_setVal(DMA1_Stream6);
				break;

			case Stream_7:
				NVIC_ISER_setVal(DMA1_Stream7);
				break;

			default:
				break;
		}
	}
	else if (DMA2 == Stream_info_st.DMAx)
	{
		switch (Stream_info_st.stream)
		{
			case Stream_0:
				NVIC_ISER_setVal(DMA2_Stream0);
				break;
			
			case Stream_1:
				NVIC_ISER_setVal(DMA2_Stream1);
				break;
			
			case Stream_2:
				NVIC_ISER_setVal(DMA2_Stream2);
				break;
			
			case Stream_3:
				NVIC_ISER_setVal(DMA2_Stream3);
				break;
			
			case Stream_4:
				NVIC_ISER_setVal(DMA2_Stream4);
				break;
			
			case Stream_5:
				NVIC_ISER_setVal(DMA2_Stream5);
				break;
			
			case Stream_6:
				NVIC_ISER_setVal(DMA2_Stream6);
				break;
			
			case Stream_7:
				NVIC_ISER_setVal(DMA2_Stream7);
				break;
			
			default:
				break;
		}
	}
}
