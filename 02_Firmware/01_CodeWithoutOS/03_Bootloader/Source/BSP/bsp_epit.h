#ifndef _BSP_EPIT_H
#define _BSP_EPIT_H

#include "MCIMX6Y2.h"
#include "fsl_common.h"
#include "fsl_iomuxc.h"
#include "core_ca7.h"
#include "fsl_gpio.h" 
#include "fsl_epit.h" 

void bsp_epit_init(void);

extern volatile bool epitIsrFlag;

#endif

