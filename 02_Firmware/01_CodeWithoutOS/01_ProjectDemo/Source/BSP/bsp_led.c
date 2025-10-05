#include "bsp_led.h"
#include <stdint.h>

void clk_enable(void)
{
	CCM_CCGR0 = 0xffffffff;
	CCM_CCGR1 = 0xffffffff;
	CCM_CCGR2 = 0xffffffff;
	CCM_CCGR3 = 0xffffffff;
	CCM_CCGR4 = 0xffffffff;
	CCM_CCGR5 = 0xffffffff;
	CCM_CCGR6 = 0xffffffff;
}

void led_init(void)
{
	SW_MUX_GPIO1_IO03 = 0x5;	/* 复用为GPIO1_IO03 */
	SW_PAD_GPIO1_IO03 = 0X10B0;		
	GPIO1_GDIR = 0X0000008;	/* GPIO1_IO03设置为输出 */                                                               
	GPIO1_DR = 0X0;
}

void led_on(void)
{
	GPIO1_DR &= ~(1<<3); 
}

void led_off(void)
{
	GPIO1_DR |= (1<<3);
}

void delay_short(volatile unsigned int n)
{
	while(n--){}
}

void delay(volatile unsigned int n)
{
	while(n--)
	{
		delay_short(0x7ff);
	}
}

