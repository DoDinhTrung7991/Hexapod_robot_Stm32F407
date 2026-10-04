#ifndef _WWDG_H_
#define _WWDG_H_

#include "stdUtility.h"

typedef enum
{
    div_1,
    div_2,
    div_4,
    div_8
} WWDG_prescaler_t;

bool WWDG_init(WWDG_prescaler_t prescaler, uint8_t timeoutMS_u8, uint8_t earlyFail_thresholdMS_u8);
void WWDG_reset(void);

#endif
