#include "bsp_led.h"

void bsp_led_init(void)
{
    gpio_pin_config_t led_config = {
        .direction = kGPIO_DigitalOutput,
        .outputLogic = 0U
    };

    // 配置引脚复用
    IOMUXC_SetPinMux(IOMUXC_GPIO1_IO03_GPIO1_IO03, 0U);

    // 配置引脚属性
    IOMUXC_SetPinConfig(IOMUXC_GPIO1_IO03_GPIO1_IO03, 0x10B0);

    // 初始化 GPIO1_IO03 为输出低电平
    GPIO_PinInit(GPIO1, 3U, &led_config);
}

void led_on(void)
{
    GPIO_WritePinOutput(GPIO1, 3U, 0U); // 设置 GPIO1_IO03 为低电平
}

void led_off(void)
{
    GPIO_WritePinOutput(GPIO1, 3U, 1U); // 设置 GPIO1_IO03 为高电平
}

void led_switch(uint8_t state)
{
	GPIO_WritePinOutput(GPIO1, 3U, state);
}

