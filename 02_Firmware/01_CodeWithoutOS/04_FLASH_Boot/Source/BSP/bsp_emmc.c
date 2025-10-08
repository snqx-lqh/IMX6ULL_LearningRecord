#include "bsp_emmc.h"
#include "bsp_uart.h"
#include "fsl_card.h"

static mmc_card_t g_mmc;

static void CardInformationLog(mmc_card_t *card);
void Test_MMC_ReadWrite(void);

void bsp_emmc_init(void)
{
    mmc_card_t *card = &g_mmc;    
    CLOCK_SetDiv(kCLOCK_Usdhc2Div, 0U);
    card->host.base = MMC_HOST_BASEADDR;
    card->host.sourceClock_Hz = MMC_HOST_CLK_FREQ;  
    card->hostVoltageWindowVCC = BOARD_MMC_VCC_SUPPLY;
 
    if (MMC_Init(&g_mmc) != kStatus_Success)
    {
        uart_printf("eMMC Init Failed!\r\n");
        return;
    }

    // EMMC 信息打印
    CardInformationLog(card);
 
    uart_printf("eMMC Init OK!\r\n");

    Test_MMC_ReadWrite();
}

void emmc_write_data(const uint8_t *buffer, uint32_t startBlock, uint32_t blockCount)
{
    MMC_WriteBlocks(&g_mmc, buffer, startBlock, blockCount);
}

void emmc_read_data(uint8_t *buffer, uint32_t startBlock, uint32_t blockCount)
{
    MMC_ReadBlocks(&g_mmc, buffer, startBlock, blockCount);
}

void Test_MMC_ReadWrite(void)
{
    uint8_t writeData[512] = "Hello eMMC!";
    uint8_t readData[512];

    MMC_WriteBlocks(&g_mmc, writeData, 80, 1);
    MMC_ReadBlocks(&g_mmc, readData, 80, 1);
    uart_printf("MMC READ:%s",readData);
}

static void CardInformationLog(mmc_card_t *card)
{
    assert(card);

    uart_printf("\r\nCard user partition size %d * %d bytes\r\n", card->blockSize, card->userPartitionBlocks);
    uart_printf("\r\nWorking condition:\r\n");
    if (card->hostVoltageWindowVCC == kMMC_VoltageWindows270to360)
    {
        uart_printf("\r\n  Voltage: VCC - 2.7V ~ 3.3V");
    }
    else if (card->hostVoltageWindowVCC == kMMC_VoltageWindow170to195)
    {
        uart_printf("\r\n  Voltage: VCC - 1.7V ~ 1.95V");
    }
    if (card->hostVoltageWindowVCCQ == kMMC_VoltageWindows270to360)
    {
        uart_printf("  VCCQ - 2.7V ~ 3.3V\r\n");
    }
    else if (card->hostVoltageWindowVCCQ == kMMC_VoltageWindow170to195)
    {
        uart_printf("  VCCQ - 1.7V ~ 1.95V\r\n");
    }
    else if (card->hostVoltageWindowVCCQ == kMMC_VoltageWindow120)
    {
        uart_printf("  VCCQ - 1.2V\r\n");
    }

    if (card->currentTiming == kMMC_HighSpeedTimingNone)
    {
        uart_printf("\r\n  Timing mode: Default");
    }
    else if ((card->currentTiming == kMMC_HighSpeed26MHZTiming) || (card->currentTiming == kMMC_HighSpeed52MHZTiming))
    {
        uart_printf("\r\n  Timing mode: High Speed\r\n");
    }
    else if (card->currentTiming == kMMC_HighSpeed200Timing)
    {
        uart_printf("\r\n  Timing mode: HS200\r\n");
    }
    else if (card->currentTiming == kMMC_HighSpeed400Timing)
    {
        uart_printf("\r\n  Timing mode: HS400\r\n");
    }
    else if (card->currentTiming == kMMC_HighSpeedDDR52Timing)
    {
        uart_printf("\r\n  Timing mode: DDR52\r\n");
    }
    if (card->currentTiming == kMMC_HighSpeedDDR52Timing)
    {
        uart_printf("\r\n  Freq : %d HZ\r\n", card->busClock_Hz / 2U);
    }
    else
    {
        uart_printf("\r\n  Freq : %d HZ\r\n", card->busClock_Hz);
    }
}
