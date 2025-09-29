
#include "bsp_clk.h"
#include "bsp_led.h"
#include "bsp_key.h"
#include "bsp_uart.h"
#include "bsp_exti.h"
#include "bsp_beep.h"
#include "bsp_epit.h"
#include "bsp_rtc.h"
#include "bsp_spi.h"
#include "bsp_emmc.h"


int main(void) {
    unsigned char a=0;
    uint8_t  led_state = 0;
	status_t ret;
 

    GIC_Init();
	/* Install IRQ Handler */
    SystemInitIrqTable();
	__set_VBAR((uint32_t)0x87800000); // 中断向量表偏移，偏移到起始地址   
	
	//int_init();
	bsp_system_clk_init();
    bsp_led_init();
	bsp_key_init();
    // 初始化 UART
    uart_init();
	bsp_exti_init();
	bsp_beep_init();
	bsp_epit_init();
	bsp_rtc_init();
	//bsp_spi_init();
	
	eMMC_Blocking_Test();

    while (1)
    {
  //      uart_puts("send one char:");
  //      a=uart_getc();
  //      uart_putc(a);	//回显功能
  //      uart_puts("\r\n");
		//if(g_InputSignal){
		//  uart_puts("key is down\r\n");
		//  g_InputSignal = false;
		//}else{
		//  uart_puts("key no down\r\n");
		//}
		if(epitIsrFlag)
		{
			epitIsrFlag = false;
			led_state = !led_state;
			led_switch(led_state);
			//get_rtc_time();
			//w25q128_read_id();
			//spi_cycle_exp();
		}
    }
	return 0;
}

/*************************** End of file ****************************/
