#ifndef BSP_LED_H
#define BSP_LED_H

#include "MCIMX6Y2.h"
#include "fsl_common.h"
#include "fsl_iomuxc.h"
#include "core_ca7.h"
#include "fsl_gpio.h" 


void bsp_led_init(void);
void led_on(void);
void led_off(void);
void led_switch(uint8_t state);

#endif

