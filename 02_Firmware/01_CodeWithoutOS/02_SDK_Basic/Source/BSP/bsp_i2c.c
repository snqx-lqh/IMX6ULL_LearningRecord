#include "bsp_i2c.h"

 
#define I2C_MASTER_CLK_FREQ (CLOCK_GetFreq(kCLOCK_IpgClk) / \
                            (CLOCK_GetDiv(kCLOCK_PerclkDiv) + 1U))

void bsp_i2c1_master_init(void)
{
    i2c_master_config_t masterConfig;
    uint32_t sourceClock;
    
    IOMUXC_SetPinConfig(IOMUXC_UART4_RX_DATA_I2C1_SDA, 
                        IOMUXC_SW_PAD_CTL_PAD_DSE(2U) |
                        IOMUXC_SW_PAD_CTL_PAD_SPEED(2U) |
                        IOMUXC_SW_PAD_CTL_PAD_PKE_MASK |
                        IOMUXC_SW_PAD_CTL_PAD_PUE_MASK |
                        IOMUXC_SW_PAD_CTL_PAD_PUS(1U));
    IOMUXC_SetPinMux(IOMUXC_UART4_TX_DATA_I2C1_SCL, 1U);
    IOMUXC_SetPinConfig(IOMUXC_UART4_TX_DATA_I2C1_SCL, 
                        IOMUXC_SW_PAD_CTL_PAD_DSE(2U) |
                        IOMUXC_SW_PAD_CTL_PAD_SPEED(2U) |
                        IOMUXC_SW_PAD_CTL_PAD_PKE_MASK |
                        IOMUXC_SW_PAD_CTL_PAD_PUE_MASK |
                        IOMUXC_SW_PAD_CTL_PAD_PUS(1U));

	/*
     * masterConfig->baudRate_Bps = 100000U;
     * masterConfig->enableHighDrive = false;
     * masterConfig->enableStopHold = false;
     * masterConfig->glitchFilterWidth = 0U;
     * masterConfig->enableMaster = true;
     */
    I2C_MasterGetDefaultConfig(&masterConfig);
    masterConfig.baudRate_Bps = 100000U;

    sourceClock = I2C_MASTER_CLK_FREQ;

    I2C_MasterInit(I2C1, &masterConfig, sourceClock);
}
 
void i2c_master_write_bytes(I2C_Type *I2C,uint8_t addr,uint8_t reg,
                                        uint8_t *data,uint32_t len)
{
    i2c_master_transfer_t masterXfer;
    memset(&masterXfer, 0, sizeof(masterXfer));

    masterXfer.slaveAddress = addr;
    masterXfer.direction = kI2C_Write;
    masterXfer.subaddress = reg;
    masterXfer.subaddressSize = 1;
    masterXfer.data = data;
    masterXfer.dataSize = len;
    masterXfer.flags = kI2C_TransferDefaultFlag;

    I2C_MasterTransferBlocking(I2C, &masterXfer);
}

void i2c_master_read_bytes(I2C_Type *I2C,uint8_t addr,uint8_t reg,
                                        uint8_t *data,uint32_t len)
{
    i2c_master_transfer_t masterXfer;
    memset(&masterXfer, 0, sizeof(masterXfer));

    masterXfer.slaveAddress = addr;
    masterXfer.direction = kI2C_Read;
    masterXfer.subaddress = reg;
    masterXfer.subaddressSize = 1;
    masterXfer.data = data;
    masterXfer.dataSize = len;
    masterXfer.flags = kI2C_TransferDefaultFlag;

    I2C_MasterTransferBlocking(I2C, &masterXfer);
}

 