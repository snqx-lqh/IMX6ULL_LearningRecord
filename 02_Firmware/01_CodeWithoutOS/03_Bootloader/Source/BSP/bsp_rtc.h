#ifndef _BSP_RTC_H
#define _BSP_RTC_H

#include "MCIMX6Y2.h"
#include "fsl_common.h"
#include "fsl_iomuxc.h"
#include "core_ca7.h"
#include "fsl_gpio.h" 
#include "fsl_snvs_hp.h" 

void bsp_rtc_init(void);
void get_rtc_time(void);
#endif
