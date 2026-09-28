#ifndef INC_DMA_H_
#define INC_DMA_H_

#include "stdUtility.h"
#include "bit_operator.h"
#include "DMA_header.h"
#include "Interrupt.h"

extern DMA_t * const DMA_reg[2];

typedef enum
{
	interruptFlag_0or4 = 0,
	interruptFlag_1or5 = 6,
	interruptFlag_2or6 = 16,
	interruptFlag_3or7 = 22
} clearFlag_shiftBit_t;

typedef enum
{
	FEIFx,
    Reserved,
    DMEIFx,
    TEIFx,
    HTIFx,
    TCIFx
} DMA_interrupt_bit_pos_t;

typedef enum
{
    DMA1,
    DMA2
} DMAx_t;

typedef enum {
    Stream_0,
    Stream_1,
    Stream_2,
    Stream_3,
    Stream_4,
    Stream_5,
    Stream_6,
    Stream_7
} Stream_t;

typedef enum
{
    single_transfer,
    INCR4,
    INCR8,
    INCR16
} BURST_t;

typedef enum
{
    disable,
    enable
} double_buffer_t;

typedef enum
{
    low,
    medium,
    high,
    very_high
} Stream_Priority_t;

typedef enum
{
    PSIZE,
    fix_32bit
} peri_inc_offset_t;

typedef enum
{
    byte,
    half_word,
    word
} data_size_t;

typedef enum
{
    fixed,
    not_fixed,
    circular
} buffer_incr_mode_t;

typedef enum
{
    peri_to_mem,
    mem_to_peri,
    mem_to_mem
} dir_t;

typedef enum
{
    DMA,
    PERIPHERAL
} flow_controller_t;

typedef enum
{
    DMEIE = 1,
    TEIE = 2,
    HTIE = 4,
    TCIE = 8
} DMA_interrupt_t;

typedef enum
{
    one_quarter,
    one_half,
    three_quarters,
    full
} FIFO_threshold_t;

typedef struct
{
    uint32_t data_length;
    volatile uint32_t *peri_addr;
    volatile uint8_t *mem_addr;
} buffer_t;

typedef struct
{
    DMAx_t DMAx;
    Stream_t stream;
    uint8_t channel;
} stream_channel_t;

typedef struct
{
    double_buffer_t double_buffer_en;
    data_size_t peri_data_size;
    data_size_t mem_data_size;
    buffer_incr_mode_t peri_mode;
    buffer_incr_mode_t mem_mode;
} direct_data_info_t;

typedef struct
{
    double_buffer_t double_buffer_en;
    BURST_t MBURST;
    BURST_t PBURST;
    peri_inc_offset_t peri_inc_offset_en;
    data_size_t peri_data_size;
    data_size_t mem_data_size;
    buffer_incr_mode_t peri_mode;
    buffer_incr_mode_t mem_mode;
} FIFO_data_info_t;

typedef struct
{
    dir_t dir;
    flow_controller_t flow_controller;
} dir_control_t;

typedef struct
{
    Stream_Priority_t stream_priority;
    uint8_t interrupt_en_u8;
} priority_interrupt_t;

typedef enum
{
    one_fourth_FIFO,
    one_second_FIFO,
    three_fourth_FIFO,
    full_FIFO
} FTH_t;

typedef struct
{
    stream_channel_t Stream_info_st;
    direct_data_info_t data_info_st;
    dir_control_t dir_control_st;
    priority_interrupt_t priority_interrupt_st;
} DMA_direct_param_t;

typedef struct
{
    stream_channel_t Stream_info_st;
    FIFO_data_info_t data_info_st;
    dir_control_t dir_control_st;
    priority_interrupt_t priority_interrupt_st;
    FTH_t FTH_en;
} DMA_FIFO_param_t;

bool DMA_direct_init(DMA_direct_param_t DMA_direct_param_st);
bool DMA_FIFO_init(DMA_FIFO_param_t DMA_FIFO_param_st);
void DMA_transfer(stream_channel_t Stream_info_st, buffer_t buffer_info_st);

#endif
