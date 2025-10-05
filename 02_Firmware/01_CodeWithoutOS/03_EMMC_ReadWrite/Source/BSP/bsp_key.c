#include "bsp_key.h"

void bsp_key_init(void)
{
	gpio_pin_config_t key_config;
	
	/* 1、初始化IO复用, 复用为GPIO1_IO18 */
	IOMUXC_SetPinMux(IOMUXC_UART1_CTS_B_GPIO1_IO18,0);

	/* 2、、配置UART1_CTS_B的IO属性	
	 *bit 16:0 HYS关闭
	 *bit [15:14]: 11 默认22K上拉
	 *bit [13]: 1 pull功能
	 *bit [12]: 1 pull/keeper使能
	 *bit [11]: 0 关闭开路输出
	 *bit [7:6]: 10 速度100Mhz
	 *bit [5:3]: 000 关闭输出
	 *bit [0]: 0 低转换率
	 */
	IOMUXC_SetPinConfig(IOMUXC_UART1_CTS_B_GPIO1_IO18,0xF080);
	
	/* 3、初始化GPIO */
	//GPIO1->GDIR &= ~(1 << 18);	/* GPIO1_IO18设置为输入 */	
	key_config.direction = kGPIO_DigitalInput;
	GPIO_PinInit(GPIO1,18, &key_config);

}

uint32_t get_key_value(void)
{
	return GPIO_ReadPinInput(GPIO1, 18);	
}

