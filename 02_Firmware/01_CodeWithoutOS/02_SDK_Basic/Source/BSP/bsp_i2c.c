#include "bsp_i2c.h"

#define EXAMPLE_I2C_MASTER_BASEADDR I2C2
#define I2C_MASTER_CLK_FREQ (CLOCK_GetFreq(kCLOCK_IpgClk) / \
                            (CLOCK_GetDiv(kCLOCK_PerclkDiv) + 1U))

#define I2C_MASTER_SLAVE_ADDR_7BIT 0x7EU
#define I2C_BAUDRATE 100000U
#define I2C_DATA_LENGTH 32U

uint8_t g_master_txBuff[I2C_DATA_LENGTH];
uint8_t g_master_rxBuff[I2C_DATA_LENGTH];
volatile bool g_MasterCompletionFlag = false;

void bsp_i2c_init(void)
{
    i2c_master_config_t masterConfig;
    uint32_t sourceClock;
    
	/*
     * masterConfig->baudRate_Bps = 100000U;
     * masterConfig->enableHighDrive = false;
     * masterConfig->enableStopHold = false;
     * masterConfig->glitchFilterWidth = 0U;
     * masterConfig->enableMaster = true;
     */
    I2C_MasterGetDefaultConfig(&masterConfig);
    masterConfig.baudRate_Bps = I2C_BAUDRATE;

    sourceClock = I2C_MASTER_CLK_FREQ;

    I2C_MasterInit(EXAMPLE_I2C_MASTER_BASEADDR, &masterConfig, sourceClock);

}

void i2c_write_bytes(void)
{
    i2c_master_transfer_t masterXfer;
    memset(&masterXfer, 0, sizeof(masterXfer));

    masterXfer.slaveAddress = I2C_MASTER_SLAVE_ADDR_7BIT;
    masterXfer.direction = kI2C_Write;
    masterXfer.subaddress = (uint32_t)NULL;
    masterXfer.subaddressSize = 0;
    masterXfer.data = g_master_txBuff;
    masterXfer.dataSize = I2C_DATA_LENGTH;
    masterXfer.flags = kI2C_TransferDefaultFlag;

    I2C_MasterTransferBlocking(EXAMPLE_I2C_MASTER_BASEADDR, &masterXfer);
}

void i2c_read_bytes(void)
{
    i2c_master_transfer_t masterXfer;
    masterXfer.slaveAddress = I2C_MASTER_SLAVE_ADDR_7BIT;
    masterXfer.direction = kI2C_Read;
    masterXfer.subaddress = (uint32_t)NULL;
    masterXfer.subaddressSize = 0;
    masterXfer.data = g_master_rxBuff;
    masterXfer.dataSize = I2C_DATA_LENGTH;

    masterXfer.flags = kI2C_TransferDefaultFlag;

    I2C_MasterTransferBlocking(EXAMPLE_I2C_MASTER_BASEADDR, &masterXfer);
}