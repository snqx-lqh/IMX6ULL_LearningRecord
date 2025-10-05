#include "bsp_uart.h"

uint32_t GetConsoleSrcFreq(void)
{
    uint32_t freq;

    /* To make it simple, we assume default PLL and divider settings, and the only variable
       from application is use PLL3 source or OSC source */
    if (CLOCK_GetMux(kCLOCK_UartMux) == 0) /* PLL3 div6 80M */
    {
        freq = (CLOCK_GetPllFreq(kCLOCK_PllUsb1) / 6U) / (CLOCK_GetDiv(kCLOCK_UartDiv) + 1U);
    }
    else
    {
        freq = CLOCK_GetOscFreq() / (CLOCK_GetDiv(kCLOCK_UartDiv) + 1U);
    }

    return freq;
}
/*
 * @description : 初始化串口1,波特率为115200
 * @param		: 无
 * @return		: 无
 */
void bsp_uart1_init(uint32_t baud)
{
    uart_config_t config;
	/* 1、初始化串口IO 			*/
	IOMUXC_SetPinMux(IOMUXC_UART1_RX_DATA_UART1_RX, 0U);
    IOMUXC_SetPinConfig(IOMUXC_UART1_RX_DATA_UART1_RX, 
                        IOMUXC_SW_PAD_CTL_PAD_DSE(2U) |
                        IOMUXC_SW_PAD_CTL_PAD_SPEED(2U) |
                        IOMUXC_SW_PAD_CTL_PAD_PKE_MASK);
    IOMUXC_SetPinMux(IOMUXC_UART1_TX_DATA_UART1_TX, 0U);
    IOMUXC_SetPinConfig(IOMUXC_UART1_TX_DATA_UART1_TX, 
                        IOMUXC_SW_PAD_CTL_PAD_DSE(2U) |
                        IOMUXC_SW_PAD_CTL_PAD_SPEED(2U) |
                        IOMUXC_SW_PAD_CTL_PAD_PKE_MASK);

	/* 2、初始化UART1  			*/
	/*
     * config.baudRate_Bps = 115200U;
     * config.parityMode = kUART_ParityDisabled;
     * config.dataBitsCount = kUART_EightDataBits;
     * config.stopBitCount = kUART_OneStopBit;
     * config.txFifoWatermark = 2;
     * config.rxFifoWatermark = 1;
     * config.enableTx = false;
     * config.enableRx = false;
     */
    UART_GetDefaultConfig(&config);
    config.baudRate_Bps = baud;
    config.enableTx = true;
    config.enableRx = true;

    UART_Init(UART1, &config, GetConsoleSrcFreq());
}


/*
 * @description : 发送一个字符
 * @param - c	: 要发送的字符
 * @return		: 无
 */
void uart_putc(unsigned char c)
{
	UART_WriteBlocking(UART1, &c, 1);
}

/*
 * @description : 发送一个字符串
 * @param - str	: 要发送的字符串
 * @return		: 无
 */
void uart_puts(char *str)
{
	char *p = str;

	while(*p)
		uart_putc(*p++);
}

/*
 * @description : 接收一个字符
 * @param 		: 无
 * @return		: 接收到的字符
 */
unsigned char uart_getc(void)
{
    unsigned char ch;
	UART_ReadBlocking(UART1, &ch, 1);
	return ch;				/* 返回接收到的数据 */
}

// 自定义的 printf 函数
void uart_printf(const char *format, ...) {
    char buffer[512];  // 定义一个足够大的缓冲区
    va_list args;

    // 初始化可变参数列表
    va_start(args, format);

    // 使用 vsnprintf 将格式化字符串转换为字符数组
    vsnprintf(buffer, sizeof(buffer), format, args);

    // 逐个字符发送
    for (size_t i = 0; buffer[i] != '\0'; i++) {
        uart_putc(buffer[i]);
    }

    // 清理可变参数列表
    va_end(args);
}

