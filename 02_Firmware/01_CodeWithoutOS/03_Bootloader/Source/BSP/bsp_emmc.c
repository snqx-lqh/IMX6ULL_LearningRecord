#include "bsp_emmc.h"
#include "bsp_uart.h"

/* 全局句柄和配置结构体 */
static usdhc_handle_t g_usdhcHandle;
static usdhc_adma_config_t g_admaConfig;
static uint32_t g_admaTable[32] __attribute__((aligned(16))); /* ADMA描述符表（16字节对齐） */

/* 回调函数实现 */
static void USDHC_TransferCompleteCallback(USDHC_Type *base, usdhc_handle_t *handle, status_t status, void *userData)
{
    if (status != kStatus_Success)
    {
        uart_printf("Transfer Error! Status: 0x%X\r\n", status);
    }
}
 

static void USDHC_CardInsertedCallback(void)
{
    uart_printf("Card Inserted!\r\n");
}

static void USDHC_CardRemovedCallback(void)
{
    uart_printf("Card Removed!\r\n");
}

/* 初始化USDHC控制器（关键修复：usdhc_config_t添加read/writeBurstLen） */
status_t eMMC_Init()
{
    usdhc_config_t config;
    status_t ret;

    /* 1. 初始化控制器配置 */
    config.endianMode = kUSDHC_EndianModeLittle;
    config.writeWatermarkLevel = 32;
    config.readWatermarkLevel = 32;
    config.dataTimeout = 0xFFFFU;
    config.readBurstLen = 8;    // 读突发长度：8
    config.writeBurstLen = 8;   // 写突发长度：8

    USDHC_Init(USDHC2, &config);

    /* 2. 初始化传输句柄和回调 */
    //USDHC_TransferCreateHandle(USDHC2, &g_usdhcHandle, USDHC_TransferCompleteCallback, NULL);
    //g_usdhcHandle.callback.CardInserted = USDHC_CardInsertedCallback;
    //g_usdhcHandle.callback.CardRemoved = USDHC_CardRemovedCallback;

    /* 3. 配置ADMA（ADMA参数正常配置，不含read/writeBurstLen） */
    g_admaConfig.dmaMode = kUSDHC_DmaModeAdma2;
    g_admaConfig.admaTable = g_admaTable;
    g_admaConfig.admaTableWords = ARRAY_SIZE(g_admaTable);
    g_admaConfig.burstLen = 8;  // ADMA基础突发长度（与控制器突发长度一致）

    /* 5. 等待eMMC卡激活 */
    if (!USDHC_SetCardActive(USDHC2, 1000U)) return kStatus_Fail;

    return kStatus_Success;
}

/* 阻塞式读块函数 */
status_t eMMC_ReadBlocks(USDHC_Type *base,
                        uint32_t startBlock,
                        uint8_t *buffer,
                        uint32_t blockCount,
                        uint32_t blockSize)
{
    usdhc_transfer_t transfer = {0};
    usdhc_command_t command = {0};
    usdhc_data_t data = {0};

    // 配置CMD18（多块读）
    command.index = 18U;
    command.argument = startBlock;  // 块地址转字节地址
    command.responseType = kCARD_ResponseTypeR1;

    // 配置数据参数
    data.blockSize = blockSize;
    data.blockCount = blockCount;
    data.rxData = (uint32_t *)buffer;  // 接收缓冲区（需32位对齐）
    data.enableAutoCommand12 = true;   // 自动发CMD12停止传输

    // 配置传输结构体
    transfer.command = &command;
    transfer.data = &data;

    // 阻塞式读取
    return USDHC_TransferBlocking(base, &g_admaConfig, &transfer);
}

/* 阻塞式写块函数 */
status_t eMMC_WriteBlocks(USDHC_Type *base,
                         uint32_t startBlock,
                         const uint8_t *buffer,
                         uint32_t blockCount,
                         uint32_t blockSize)
{
    usdhc_transfer_t transfer = {0};
    usdhc_command_t command = {0};
    usdhc_data_t data = {0};

    // 配置CMD25（多块写）
    command.index = 25U;
    command.argument = startBlock;
    command.responseType = kCARD_ResponseTypeR1;

    // 配置数据参数
    data.blockSize = blockSize;
    data.blockCount = blockCount;
    data.txData = (const uint32_t *)buffer;  // 发送缓冲区（需32位对齐）
    data.enableAutoCommand12 = true;         // 自动发CMD12停止传输
    data.enableAutoCommand23 = true;         // 自动发CMD23预配置块数

    // 配置传输结构体
    transfer.command = &command;
    transfer.data = &data;

    // 阻塞式写入
    return USDHC_TransferBlocking(base, &g_admaConfig, &transfer);
}

/* 阻塞式读写测试示例 */
#define TEST_START_BLOCK  0x100U    // 测试起始块（避开引导分区）
#define TEST_BLOCK_COUNT  4U        // 测试块数（4×512=2048字节）
#define TEST_BLOCK_SIZE   512U      // eMMC标准块大小

// 非缓存区缓冲区（避免ADMA访问缓存脏数据）
static uint8_t g_writeBuf[TEST_BLOCK_COUNT * TEST_BLOCK_SIZE] __attribute__((section(".noncacheable")));
static uint8_t g_readBuf[TEST_BLOCK_COUNT * TEST_BLOCK_SIZE] __attribute__((section(".noncacheable")));

status_t eMMC_Blocking_Test()
{
    status_t ret;
    uint32_t i;

    // 1. 初始化eMMC
    uart_printf("eMMC Init...\r\n");
    ret = eMMC_Init();
    if (ret != kStatus_Success)
    {
        uart_printf("Init Fail! Status: 0x%X\r\n", ret);
        return ret;
    }

    // 2. 填充测试数据
    uart_printf("Fill Write Buffer...\r\n");
    for (i = 0; i < sizeof(g_writeBuf); i++)
    {
        g_writeBuf[i] = i % 0x10;  // 0x00~0x0F循环填充
    }

    // 3. 阻塞式写入
    uart_printf("Write: Block=0x%X, Count=%d\r\n", TEST_START_BLOCK, TEST_BLOCK_COUNT);
    ret = eMMC_WriteBlocks(USDHC2, TEST_START_BLOCK, g_writeBuf, TEST_BLOCK_COUNT, TEST_BLOCK_SIZE);
    if (ret != kStatus_Success)
    {
        uart_printf("Write Fail! Status: 0x%X\r\n", ret);
        return ret;
    }

    // 4. 阻塞式读取
    memset(g_readBuf, 0, sizeof(g_readBuf));
    uart_printf("Read: Block=0x%X, Count=%d\r\n", TEST_START_BLOCK, TEST_BLOCK_COUNT);
    ret = eMMC_ReadBlocks(USDHC2, TEST_START_BLOCK, g_readBuf, TEST_BLOCK_COUNT, TEST_BLOCK_SIZE);
    if (ret != kStatus_Success)
    {
        uart_printf("Read Fail! Status: 0x%X\r\n", ret);
        return ret;
    }

    // 5. 校验数据
    uart_printf("Verify Data...\r\n");
    for (i = 0; i < sizeof(g_readBuf); i++)
    {
        if (g_readBuf[i] != g_writeBuf[i])
        {
            uart_printf("Verify Fail! Index=%d, Write=0x%X, Read=0x%X\r\n", i, g_writeBuf[i], g_readBuf[i]);
            return kStatus_Fail;
        }
    }

    uart_printf("Test Pass!\r\n");
    return kStatus_Success;
}

 