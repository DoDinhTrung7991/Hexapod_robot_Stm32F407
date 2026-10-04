#ifndef _IWDG_H_
#define _IWDG_H_

#include "stdUtility.h"

typedef enum
{
    _500ms,
    _1000ms,
    _2000ms,
    _4000ms,
    _8000ms,
    _16000ms,
    _32000ms
} IWDG_timeout_t;

bool IWDG_init(IWDG_timeout_t timeout);
void IWDG_reset(void);

#endif