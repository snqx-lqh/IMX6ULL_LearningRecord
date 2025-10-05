#include "bsp_clk.h"
#include "bsp_led.h"
#include "bsp_key.h"
#include "bsp_uart.h"
#include "bsp_exti.h"
#include "bsp_beep.h"
#include "bsp_epit.h"
#include "bsp_rtc.h"
#include "bsp_delay.h"
#include "bsp_i2c.h"
#include "bsp_spi.h"
#include "bsp_emmc.h"

int main(void) {
    unsigned char a=0;
    uint8_t  led_state = 0;
	int ret;
    
    GIC_Init();
    SystemInitIrqTable();             //初始化中断向量表
	__set_VBAR((uint32_t)0x87800000); // 中断向量表偏移，偏移到起始地址   
	
	bsp_system_clk_init();
    delay_init();
    bsp_led_init();
	bsp_key_init();
    // 初始化 UART
    bsp_uart1_init(115200);
	bsp_exti_init();
	bsp_beep_init();
	bsp_epit1_init(1000000);
	bsp_rtc_init();
    bsp_i2c1_master_init();
	bsp_spi_init();
    bsp_emmc_init();

    uart_printf("all init \r\n");
    
    while (1)
    {
 
    }
	return 0;
}
 
void EPIT1_IRQHander_Callback(void)
{
    static int led_state = 0;
    led_state = !led_state;
    led_switch(led_state);
}