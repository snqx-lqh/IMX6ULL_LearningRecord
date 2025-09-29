
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

/* 硬件配置参数 */
#define USDHC_INSTANCE     USDHC2          // USDHC控制器实例
#define CLOCK_SOURCE       240000000U      // 时钟源频率(Hz)
#define TARGET_CLOCK       52000000U       // 目标时钟频率(52MHz)
#define BLOCK_SIZE         512U            // 块大小(与mmc info一致)
#define TEST_BLOCK_ADDR    0x1000U         // 测试块地址(确保未使用)
#define TEST_BLOCK_COUNT   1U              // 测试块数量

/* 全局缓冲区 */
uint8_t tx_buffer[BLOCK_SIZE];
uint8_t rx_buffer[BLOCK_SIZE];

/* 声明外部API函数 */
status_t eMMC_Init(USDHC_Type *base);
status_t eMMC_ReadBlocks(USDHC_Type *base, uint32_t startBlock, uint8_t *buffer, 
                        uint32_t blockCount, uint32_t blockSize);
status_t eMMC_WriteBlocks(USDHC_Type *base, uint32_t startBlock, const uint8_t *buffer, 
                         uint32_t blockCount, uint32_t blockSize);
status_t eMMC_SetClock(USDHC_Type *base, uint32_t srcClock_Hz, uint32_t targetClock_Hz);

/* 填充测试数据 */
static void fill_test_data(uint8_t *buf, uint32_t size)
{
    for (uint32_t i = 0; i < size; i++)
    {
        buf[i] = (uint8_t)(i % 256);  // 生成0-255循环的测试数据
    }
}

/* 打印缓冲区数据（前32字节） */
static void print_buffer(const char *title, const uint8_t *buf, uint32_t len)
{
    uart_printf("%s: ", title);
    len = (len > 32) ? 32 : len;  // 最多打印32字节
    for (uint32_t i = 0; i < len; i++)
    {
        uart_printf("%02X ", buf[i]);
    }
    uart_printf("\r\n");
}

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
	
	Emmc_Init();

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
