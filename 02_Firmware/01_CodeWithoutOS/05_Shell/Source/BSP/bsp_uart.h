#ifndef _BSP_UART_H
#define _BSP_UART_H

#include "MCIMX6Y2.h"
#include "fsl_common.h"
#include "fsl_iomuxc.h"
#include "core_ca7.h"
#include "fsl_uart.h" 

#include <stdarg.h>
#include <stdio.h>


void bsp_uart1_init(uint32_t baud);
void uart_putc(unsigned char c);
void uart_puts(char *str);
unsigned char uart_getc(void);
void uart_printf(const char *format, ...);

#endif
