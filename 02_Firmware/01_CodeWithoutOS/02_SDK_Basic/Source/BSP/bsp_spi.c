#include "bsp_spi.h"
 
#define ECSPI_MASTER_CLK_FREQ (CLOCK_GetPllFreq(kCLOCK_PllUsb1) / 8U)
 
void spi_cycle_exp(void);

void bsp_spi_init(void)
{
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

//   IOMUXC_SetPinMux(IOMUXC_UART2_TX_DATA_ECSPI3_SS0, 0U);
//   IOMUXC_SetPinConfig(IOMUXC_UART2_TX_DATA_ECSPI3_SS0, 
//                       IOMUXC_SW_PAD_CTL_PAD_DSE(2U) |
//                       IOMUXC_SW_PAD_CTL_PAD_SPEED(2U) |
//                       IOMUXC_SW_PAD_CTL_PAD_PUE_MASK |
//                       IOMUXC_SW_PAD_CTL_PAD_PUS(2U) |
//                       IOMUXC_SW_PAD_CTL_PAD_HYS_MASK);            
                
	gpio_pin_config_t led_config = {
        .direction = kGPIO_DigitalOutput,
        .outputLogic = 0U
    };

    // 配置引脚复用
    IOMUXC_SetPinMux(IOMUXC_UART2_TX_DATA_GPIO1_IO20, 0U);
    IOMUXC_SetPinConfig(IOMUXC_UART2_TX_DATA_GPIO1_IO20, IOMUXC_SW_PAD_CTL_PAD_DSE(7U) | 
                                                         IOMUXC_SW_PAD_CTL_PAD_PUS(3U));
    GPIO_PinInit(GPIO1, 20U, &led_config);

	ecspi_master_config_t masterConfig;
	ECSPI_MasterGetDefaultConfig(&masterConfig);
    masterConfig.baudRate_Bps = 18000000U;

    ECSPI_MasterInit(ECSPI3, &masterConfig, ECSPI_MASTER_CLK_FREQ);
    
    //回环测试
	//spi_cycle_exp();
}

uint8_t spi_read_write_byte(uint8_t txByte)
{
	uint32_t tx = txByte;   // 注意这里用 uint32_t
    uint32_t rx = 0;
	ecspi_transfer_t masterXfer;
	masterXfer.txData = &tx;
	masterXfer.rxData = &rx;
	masterXfer.dataSize = 1;
	masterXfer.channel = kECSPI_Channel0;
	ECSPI_MasterTransferBlocking(ECSPI3, &masterXfer);
	return  (uint8_t)rx;
}

 