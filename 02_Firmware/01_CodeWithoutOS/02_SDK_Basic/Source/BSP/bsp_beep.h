#ifndef _BSP_BEEP_H
#define _BSP_BEEP_H

#include "MCIMX6Y2.h"
#include "fsl_common.h"
#include "fsl_iomuxc.h"
#include "core_ca7.h"
#include "fsl_gpio.h" 

void bsp_beep_init(void);
void beep_switch(uint8_t state);

#endif
