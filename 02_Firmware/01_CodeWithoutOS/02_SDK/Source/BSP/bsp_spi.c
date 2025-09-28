#include "bsp_spi.h"

#include "bsp_uart.h"
 

#define EXAMPLE_ECSPI_MASTER_BASEADDR ECSPI3
#define EXAMPLE_ECSPI_DEALY_COUNT 1000000U
#define ECSPI_MASTER_CLK_FREQ (CLOCK_GetPllFreq(kCLOCK_PllUsb1) / 8U)
#define TRANSFER_SIZE 64U        
#define TRANSFER_BAUDRATE 18000000U  

uint32_t masterRxData[TRANSFER_SIZE] = {0U};
uint32_t masterTxData[TRANSFER_SIZE] = {0U};

#define W25Q128_CMD_READ_JEDEC_ID       0x9F
#define W25Q128_CMD_READ_ID       0x90
#define W25Q128_CMD_WRITE_ENABLE  0x06
#define W25Q128_CMD_PAGE_PROGRAM  0x02
#define W25Q128_CMD_READ_DATA     0x03

void w25q128_read_id(void) ;
void spi_cycle_exp(void);

void bsp_spi_init(void)
{
	//IOMUXC_SetPinMux(IOMUXC_UART2_TX_DATA_ECSPI3_SS0, 0U);
 //   IOMUXC_SetPinConfig(IOMUXC_UART2_TX_DATA_ECSPI3_SS0, 
 //                       IOMUXC_SW_PAD_CTL_PAD_DSE(2U) |
 //                       IOMUXC_SW_PAD_CTL_PAD_SPEED(2U) |
 //                       IOMUXC_SW_PAD_CTL_PAD_PUE_MASK |
 //                       IOMUXC_SW_PAD_CTL_PAD_PUS(2U) |
 //                       IOMUXC_SW_PAD_CTL_PAD_HYS_MASK);
    IOMUXC_SetPinMux(IOMUXC_UART2_RTS_B_ECSPI3_MISO, 0U);
    IOMUXC_SetPinConfig(IOMUXC_UART2_RTS_B_ECSPI3_MISO, 
                        IOMUXC_SW_PAD_CTL_PAD_DSE(2U) |
                        IOMUXC_SW_PAD_CTL_PAD_SPEED(2U) |
                        IOMUXC_SW_PAD_CTL_PAD_HYS_MASK);
    IOMUXC_SetPinMux(IOMUXC_UART2_RX_DATA_ECSPI3_SCLK, 0U);
    IOMUXC_SetPinConfig(IOMUXC_UART2_RX_DATA_ECSPI3_SCLK, 
                        IOMUXC_SW_PAD_CTL_PAD_DSE(2U) |
                        IOMUXC_SW_PAD_CTL_PAD_SPEED(2U) |
                        IOMUXC_SW_PAD_CTL_PAD_HYS_MASK);
    IOMUXC_SetPinMux(IOMUXC_UART2_CTS_B_ECSPI3_MOSI, 0U);
    IOMUXC_SetPinConfig(IOMUXC_UART2_CTS_B_ECSPI3_MOSI, 
                        IOMUXC_SW_PAD_CTL_PAD_DSE(2U) |
                        IOMUXC_SW_PAD_CTL_PAD_SPEED(2U) |
                        IOMUXC_SW_PAD_CTL_PAD_HYS_MASK);
		
	gpio_pin_config_t led_config = {
        .direction = kGPIO_DigitalOutput,
        .outputLogic = 0U
    };

    // 配置引脚复用
    IOMUXC_SetPinMux(IOMUXC_UART2_TX_DATA_GPIO1_IO20, 0U);

    // 配置引脚属性
    IOMUXC_SetPinConfig(IOMUXC_UART2_TX_DATA_GPIO1_IO20, IOMUXC_SW_PAD_CTL_PAD_DSE(7U) | IOMUXC_SW_PAD_CTL_PAD_PUS(3U));

    // 初始化 GPIO1_IO03 为输出低电平
    GPIO_PinInit(GPIO1, 20U, &led_config);

	ecspi_master_config_t masterConfig;
	ECSPI_MasterGetDefaultConfig(&masterConfig);
    masterConfig.baudRate_Bps = TRANSFER_BAUDRATE;

    ECSPI_MasterInit(EXAMPLE_ECSPI_MASTER_BASEADDR, &masterConfig, ECSPI_MASTER_CLK_FREQ);

	//w25q128_read_id();
	spi_cycle_exp();
}

uint8_t spi_read_write_byte(uint8_t txByte)
{
	uint32_t tx = txByte;   // 注意这里用 uint32_t
    uint32_t rx = 0;
	ecspi_transfer_t masterXfer;
/* Start master transfer, send data to slave */
	masterXfer.txData = &tx;
	masterXfer.rxData = &rx;
	masterXfer.dataSize = 1;
	masterXfer.channel = kECSPI_Channel0;
	ECSPI_MasterTransferBlocking(EXAMPLE_ECSPI_MASTER_BASEADDR, &masterXfer);
	return  (uint8_t)rx;
}

void w25q128_read_id(void) {
 //   int i = 10000;
	//GPIO_WritePinOutput(GPIO1, 20U, 0U); // 设置 GPIO1_IO03 为低电平
 //   spi_read_write_byte(0x66);
 //   spi_read_write_byte(0x99);
 //   GPIO_WritePinOutput(GPIO1, 20U, 1U); // 设置 GPIO1_IO03 为低电平
	//uart_printf("W25Q128 RESET\n");
	//while(i--);
	uint8_t manufacturer, memory_type, capacity;
    GPIO_WritePinOutput(GPIO1, 20U, 0U); // 设置 GPIO1_IO03 为低电平
    spi_read_write_byte(W25Q128_CMD_READ_JEDEC_ID);
    manufacturer = spi_read_write_byte(0xFF);
    memory_type  = spi_read_write_byte(0xFF);
    capacity     = spi_read_write_byte(0xFF);
    GPIO_WritePinOutput(GPIO1, 20U, 1U); // 设置 GPIO1_IO03 为低电平
	uart_printf("W25Q128 JEDEC ID: %X %X %X\n", manufacturer, memory_type, capacity);
	
	//uint8_t manufacturer, memory_type;
	GPIO_WritePinOutput(GPIO1, 20U, 0U); // 设置 GPIO1_IO03 为低电平
	spi_read_write_byte(W25Q128_CMD_READ_ID);
	spi_read_write_byte(0x00);  // 高地址
	spi_read_write_byte(0x00);  // 低地址
	spi_read_write_byte(0x00);  // 低地址
	manufacturer = spi_read_write_byte(0xFF);
	memory_type  = spi_read_write_byte(0xFF);
	GPIO_WritePinOutput(GPIO1, 20U, 1U); // 设置 GPIO1_IO03 为低电平

	uart_printf("Manufacturer/Device ID: %X %X\n", manufacturer, memory_type);  
}

void spi_cycle_exp(void) {
 
	uint8_t manufacturer, memory_type, capacity;
    GPIO_WritePinOutput(GPIO1, 20U, 0U); // 设置 GPIO1_IO03 为低电平
    manufacturer = spi_read_write_byte(0x78);
    GPIO_WritePinOutput(GPIO1, 20U, 1U); // 设置 GPIO1_IO03 为低电平
	uart_printf("W25Q128 JEDEC ID: %X\n", manufacturer);
}