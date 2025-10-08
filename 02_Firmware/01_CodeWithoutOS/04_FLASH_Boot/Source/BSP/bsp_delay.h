#ifndef __BSP_DELAY_H
#define __BSP_DELAY_H
 
#include "MCIMX6Y2.h"
#include "fsl_common.h"
#include "fsl_iomuxc.h"
#include "core_ca7.h"
#include "fsl_gpt.h"

/* 函数声明 */
void delay_init(void);
void delayus(unsigned    int usdelay);
void delayms(unsigned	 int msdelay);
 
 

#endif

