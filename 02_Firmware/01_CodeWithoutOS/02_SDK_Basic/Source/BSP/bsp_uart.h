#ifndef _BSP_UART_H
#define _BSP_UART_H

#include "MCIMX6Y2.h"
#include "fsl_common.h"
#include "fsl_iomuxc.h"
#include "core_ca7.h"
#include "fsl_gpio.h" 

/***************************************************************
Copyright © zuozhongkai Co., Ltd. 1998-2019. All rights reserved.
文件名	: 	 bsp_led.c
作者	   : 左忠凯
版本	   : V1.0
描述	   : 串口驱动文件头文件。
其他	   : 无
论坛 	   : www.wtmembed.com
日志	   : 初版V1.0 2019/1/15 左忠凯创建
***************************************************************/

/* 函数声明 */
void uart_init(void);
void uart_io_init(void);
void uart_disable(UART_Type *base);
void uart_enable(UART_Type *base);
void uart_softreset(UART_Type *base);
void uart_setbaudrate(UART_Type *base, unsigned int baudrate, unsigned int srcclock_hz);
void uart_putc(unsigned char c);
void uart_puts(char *str);
unsigned char uart_getc(void);
void raise(int sig_nr);
void uart_printf(const char *format, ...);

#endif
