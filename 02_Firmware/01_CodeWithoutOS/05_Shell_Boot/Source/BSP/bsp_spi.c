#include "bsp_spi.h"
#include "bsp_uart.h"

#define ECSPI_MASTER_CLK_FREQ (CLOCK_GetPllFreq(kCLOCK_PllUsb1) / 8U)
#define TRANSFER_SIZE 3U         /*! Transfer dataSize */
#define TRANSFER_BAUDRATE 500000U /*! Transfer baudrate - 500k 18000000*/ 

/*******************************************************************************
 * Variables
 ******************************************************************************/
uint32_t masterRxData[TRANSFER_SIZE] = {0U};
uint32_t masterTxData[TRANSFER_SIZE] = {0U};


void spi_cycle_exp(void);

ecspi_master_handle_t g_m_handle;
volatile bool isTransferCompleted = false;

/*******************************************************************************
 * Code
 ******************************************************************************/

void ECSPI_MasterUserCallback(ECSPI_Type *base, ecspi_master_handle_t *handle, status_t status, void *userData)
{
    if (status == kStatus_Success)
    {
        isTransferCompleted = true;
    }

    if (status == kStatus_ECSPI_HardwareOverFlow)
    {
        uart_printf("Hardware overflow occured in this transfer. \r\n");
    }
}

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

   //IOMUXC_SetPinMux(IOMUXC_UART2_TX_DATA_ECSPI3_SS0, 0U);
   //IOMUXC_SetPinConfig(IOMUXC_UART2_TX_DATA_ECSPI3_SS0, 
   //                    IOMUXC_SW_PAD_CTL_PAD_DSE(2U) |
   //                    IOMUXC_SW_PAD_CTL_PAD_SPEED(2U) |
   //                    IOMUXC_SW_PAD_CTL_PAD_PUE_MASK |
   //                    IOMUXC_SW_PAD_CTL_PAD_PUS(2U) |
   //                    IOMUXC_SW_PAD_CTL_PAD_HYS_MASK);            
                
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
    masterConfig.burstLength = 8;
    masterConfig.baudRate_Bps = TRANSFER_BAUDRATE;

    ECSPI_MasterInit(ECSPI3, &masterConfig, ECSPI_MASTER_CLK_FREQ);
    ECSPI_MasterTransferCreateHandle(ECSPI3, &g_m_handle, ECSPI_MasterUserCallback, NULL);

    //回环测试
	//spi_cycle_exp();
}

unsigned char spich0_readwrite_byte(ECSPI_Type *base,unsigned char txdata)
{
    uint32_t spirxdata = 0;
    uint32_t spitxdata = txdata;
    
    /* 选择通道 0 */
    base->CONREG &= ~(3 << 18);
    base->CONREG |= (0 << 18);
    
    while((base->STATREG & (1 << 0)) == 0){} /* 等待发送 FIFO 为空 */
    base->TXDATA = spitxdata;
    
    while((base->STATREG & (1 << 3)) == 0){} /* 等待接收 FIFO 有数据 */
    spirxdata = base->RXDATA;
    return spirxdata;
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

void spi_read_write_bytes(uint32_t *txBuffer,uint32_t *rxBuffer,uint32_t len)
{
	ecspi_transfer_t masterXfer;
	masterXfer.txData = txBuffer;
	masterXfer.rxData = rxBuffer;
	masterXfer.dataSize = len;
	masterXfer.channel = kECSPI_Channel0;
	ECSPI_MasterTransferBlocking(ECSPI3, &masterXfer);
}

void spi_read_write_bytes_it(uint32_t *txBuffer,uint32_t *rxBuffer,uint32_t len)
{
	ecspi_transfer_t masterXfer;
	masterXfer.txData = txBuffer;
	masterXfer.rxData = rxBuffer;
	masterXfer.dataSize = len;
	masterXfer.channel = kECSPI_Channel0;
	ECSPI_MasterTransferNonBlocking(ECSPI3, &g_m_handle, &masterXfer);

    /* Wait transfer complete */
    while (!isTransferCompleted)
    {
    }
}
 