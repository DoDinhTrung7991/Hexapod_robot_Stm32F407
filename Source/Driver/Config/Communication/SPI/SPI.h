#ifndef _SPI_H_
#define _SPI_H_

#include "SPI_header.h"
#include "DMA.h"
#include "queue.h"
#include "init_static.h"
#include "Interrupt.h"

typedef struct
{
    GPIO_ENABLE_t GPIOx_en;
    uint8_t pos_u8;
} GPIO_SPI_t;

typedef enum
{
    SPI1,
    SPI2,
    SPI3
} SPIx_t;

typedef enum
{
    master,
    slave
} SPI_mode_t;

typedef enum
{
    mode1,
    mode2,
    mode3,
    mode4
} SPI_sampling_mode_t;

typedef enum
{
    _4MHZ,
    _2MHZ,
    _1MHZ,
    _500kHz,
    _250kHz,
    _125kHz,
    _62500Hz,
    _31250Hz
} SPI_baudrate_t;

bool SPI_init(SPIx_t SPIx_en, SPI_mode_t mode_en, SPI_sampling_mode_t SPI_sampling_mode_en, SPI_baudrate_t baudrate_t);

#endif
