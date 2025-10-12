#ifndef _BSP_EXIT_H
#define _BSP_EXIT_H

#include "MCIMX6Y2.h"
#include "fsl_common.h"
#include "fsl_iomuxc.h"
#include "core_ca7.h"
#include "fsl_gpio.h" 


/* 函数声明 */
void bsp_exti_init(void);					/* 中断初始化 */
void gpio1_io18_irqhandler(void); 			/* 中断处理函数 */

 

#endif
