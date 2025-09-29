#include "bsp_beep.h"

void bsp_beep_init(void)
{
    gpio_pin_config_t beep_config = {
        .direction = kGPIO_DigitalOutput,
        .outputLogic = 1U
    };

    // 配置引脚复用
    IOMUXC_SetPinMux(IOMUXC_SNVS_SNVS_TAMPER1_GPIO5_IO01, 0U);

    // 配置引脚属性
    IOMUXC_SetPinConfig(IOMUXC_SNVS_SNVS_TAMPER1_GPIO5_IO01, 0X10B0);

    // 初始化 GPIO5_IO01 为输出低电平
    GPIO_PinInit(GPIO5, 1U, &beep_config);

}

void beep_switch(uint8_t state)
{
	GPIO_WritePinOutput(GPIO5, 1U, state);
}