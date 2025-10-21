#include "bsp_clk.h"
#include "bsp_led.h"
#include "bsp_key.h"
#include "bsp_uart.h"
#include "bsp_exti.h"
#include "bsp_beep.h"
//#include "bsp_epit.h"
//#include "bsp_rtc.h"
#include "bsp_delay.h"
//#include "bsp_i2c.h"
#include "bsp_spi.h"
//#include "bsp_emmc.h"
#include "board.h"
#include "w25q128.h"
#include "xmodem.h"

#include "shell.h"

#pragma data_alignment=4

const char version_str[] __attribute__((aligned(4))) = "V1.0.0";

int main(void) {
    unsigned char a=0;
    uint8_t  led_state = 0;
	int ret;
    uint8_t xmodem_start_flag=0;
    uint16_t w25_id = 0;
    char version_str1[10] = "V1.0.1";

    GIC_Init();
    SystemInitIrqTable();             //初始化中断向量表
	__set_VBAR((uint32_t)0x87800000); // 中断向量表偏移，偏移到起始地址   
	
	bsp_system_clk_init();
    BOARD_InitMemory();
    delay_init();
    bsp_led_init();
	bsp_key_init();
    // 初始化 UART
    bsp_uart1_init(115200);
	//bsp_exti_init();
	//bsp_beep_init();
	//bsp_epit1_init(1000000);
	//bsp_rtc_init();
    //bsp_i2c1_master_init();
	bsp_spi_init();
    //bsp_emmc_init();
 
    uart_printf("all init \r\n");
    
    //uart_printf("flash bootloader version V1.0.6\r\n");
    uart_printf("bootloader version V1.0.7\r\n");
    //uart_printf("build date:%s time:%s\r\n",__DATE__,__TIME__);
    
    while(1)
	{
		w25_id  = W25QXX_ReadID();
        if (w25_id == W25Q128 || w25_id == NM25Q128)
        {
            uart_printf("w25_id is W25Q128\r\n");
            break;
        }else if (w25_id == W25Q256 || w25_id == NM25Q256)
        {
            uart_printf("w25_id is W25Q256\r\n");
            break;
        }else{
            uart_printf("w25 init failed,w25_id is %x\r\n",w25_id);
        }
	}
    
    shell_task();
    
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

void HardFault_Handler(void)
{
    uart_printf("HardFault! \n"  );
    while (1);
}
