 /***************************************************************
Copyright © zuozhongkai Co., Ltd. 1998-2019. All rights reserved.
文件名	: 	 bsp_exit.c
作者	   : 左忠凯
版本	   : V1.0
描述	   : 外部中断驱动。
其他	   : 配置按键对应的GPIP为中断模式
论坛 	   : www.wtmembed.com
日志	   : 初版V1.0 2019/1/4 左忠凯创建
***************************************************************/
#include "bsp_exti.h"
#include "bsp_led.h"

volatile bool g_InputSignal = false;

/*
 * @description			: 初始化外部中断
 * @param				: 无
 * @return 				: 无
 */
void bsp_exti_init(void)
{
	gpio_pin_config_t key_config;

	/* 1、设置IO复用 */
	IOMUXC_SetPinMux(IOMUXC_UART1_CTS_B_GPIO1_IO18,0);			/* 复用为GPIO1_IO18 */
	IOMUXC_SetPinConfig(IOMUXC_UART1_CTS_B_GPIO1_IO18,0xF080);

	/* 2、初始化GPIO为中断模式 */
	key_config.direction = kGPIO_DigitalInput;
	key_config.interruptMode = kGPIO_IntFallingEdge;
	key_config.outputLogic = 1;
	GPIO_PinInit(GPIO1, 18, &key_config);

	GIC_EnableIRQ(GPIO1_Combined_16_31_IRQn);				/* 使能GIC中对应的中断 */
	SystemInstallIrqHandler(GPIO1_Combined_16_31_IRQn, (system_irq_handler_t)gpio1_io18_irqhandler, NULL);
	GPIO_EnableInterrupts(GPIO1, 1 << 18);					/* 使能GPIO1_IO18的中断功能 */
}

/*
 * @description			: GPIO1_IO18最终的中断处理函数
 * @param				: 无
 * @return 				: 无
 */
void gpio1_io18_irqhandler(void)
{ 
	static unsigned char state = 0;
	if(GPIO_ReadPinInput(GPIO1, 18) == 0)	/* 按键按下了  */
	{
		state = !state;
		led_switch(state);
	}
	 GPIO_ClearPinsInterruptFlags(GPIO1, 1U << 18);
	 g_InputSignal = true;
	  
}



