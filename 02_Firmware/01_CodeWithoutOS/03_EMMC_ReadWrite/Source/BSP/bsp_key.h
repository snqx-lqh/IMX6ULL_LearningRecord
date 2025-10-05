#ifndef _BSP_KEY_H
#define _BSP_KEY_H

#include "MCIMX6Y2.h"
#include "fsl_common.h"
#include "fsl_iomuxc.h"
#include "core_ca7.h"
#include "fsl_gpio.h" 

void bsp_key_init(void);
uint32_t get_key_value(void);

#endif
