#include "bsp_emmc.h"
#include "bsp_uart.h"
#include "fsl_card.h"

static mmc_card_t g_mmc;

static void CardInformationLog(mmc_card_t *card);
void Test_MMC_ReadWrite(void);

void emmc_pinmux_init(void)
{
    IOMUXC_SetPinMux(IOMUXC_GPIO1_IO09_GPIO1_IO09, 0U);
    IOMUXC_SetPinConfig(IOMUXC_GPIO1_IO09_GPIO1_IO09, 
                        IOMUXC_SW_PAD_CTL_PAD_SRE_MASK |
                        IOMUXC_SW_PAD_CTL_PAD_DSE(1U) |
                        IOMUXC_SW_PAD_CTL_PAD_HYS_MASK);
    IOMUXC_SetPinMux(IOMUXC_NAND_RE_B_USDHC2_CLK, 0U);
    IOMUXC_SetPinConfig(IOMUXC_NAND_RE_B_USDHC2_CLK, 
                        IOMUXC_SW_PAD_CTL_PAD_SRE_MASK |
                        IOMUXC_SW_PAD_CTL_PAD_DSE(1U) |
                        IOMUXC_SW_PAD_CTL_PAD_SPEED(1U) |
                        IOMUXC_SW_PAD_CTL_PAD_PKE_MASK |
                        IOMUXC_SW_PAD_CTL_PAD_PUE_MASK |
                        IOMUXC_SW_PAD_CTL_PAD_PUS(1U) |
                        IOMUXC_SW_PAD_CTL_PAD_HYS_MASK);
    IOMUXC_SetPinMux(IOMUXC_NAND_WE_B_USDHC2_CMD, 0U);
    IOMUXC_SetPinConfig(IOMUXC_NAND_WE_B_USDHC2_CMD, 
                        IOMUXC_SW_PAD_CTL_PAD_SRE_MASK |
                        IOMUXC_SW_PAD_CTL_PAD_DSE(1U) |
                        IOMUXC_SW_PAD_CTL_PAD_SPEED(2U) |
                        IOMUXC_SW_PAD_CTL_PAD_PKE_MASK |
                        IOMUXC_SW_PAD_CTL_PAD_PUE_MASK |
                        IOMUXC_SW_PAD_CTL_PAD_PUS(1U) |
                        IOMUXC_SW_PAD_CTL_PAD_HYS_MASK);
    IOMUXC_SetPinMux(IOMUXC_NAND_DATA00_USDHC2_DATA0, 0U);
    IOMUXC_SetPinConfig(IOMUXC_NAND_DATA00_USDHC2_DATA0, 
                        IOMUXC_SW_PAD_CTL_PAD_SRE_MASK |
                        IOMUXC_SW_PAD_CTL_PAD_DSE(1U) |
                        IOMUXC_SW_PAD_CTL_PAD_SPEED(2U) |
                        IOMUXC_SW_PAD_CTL_PAD_PKE_MASK |
                        IOMUXC_SW_PAD_CTL_PAD_PUE_MASK |
                        IOMUXC_SW_PAD_CTL_PAD_PUS(1U) |
                        IOMUXC_SW_PAD_CTL_PAD_HYS_MASK);
    IOMUXC_SetPinMux(IOMUXC_NAND_DATA01_USDHC2_DATA1, 0U);
    IOMUXC_SetPinConfig(IOMUXC_NAND_DATA01_USDHC2_DATA1, 
                        IOMUXC_SW_PAD_CTL_PAD_SRE_MASK |
                        IOMUXC_SW_PAD_CTL_PAD_DSE(1U) |
                        IOMUXC_SW_PAD_CTL_PAD_SPEED(2U) |
                        IOMUXC_SW_PAD_CTL_PAD_PKE_MASK |
                        IOMUXC_SW_PAD_CTL_PAD_PUE_MASK |
                        IOMUXC_SW_PAD_CTL_PAD_PUS(1U) |
                        IOMUXC_SW_PAD_CTL_PAD_HYS_MASK);
    IOMUXC_SetPinMux(IOMUXC_NAND_DATA02_USDHC2_DATA2, 0U);
    IOMUXC_SetPinConfig(IOMUXC_NAND_DATA02_USDHC2_DATA2, 
                        IOMUXC_SW_PAD_CTL_PAD_SRE_MASK |
                        IOMUXC_SW_PAD_CTL_PAD_DSE(1U) |
                        IOMUXC_SW_PAD_CTL_PAD_SPEED(2U) |
                        IOMUXC_SW_PAD_CTL_PAD_PKE_MASK |
                        IOMUXC_SW_PAD_CTL_PAD_PUE_MASK |
                        IOMUXC_SW_PAD_CTL_PAD_PUS(1U) |
                        IOMUXC_SW_PAD_CTL_PAD_HYS_MASK);
    IOMUXC_SetPinMux(IOMUXC_NAND_DATA03_USDHC2_DATA3, 0U);
    IOMUXC_SetPinConfig(IOMUXC_NAND_DATA03_USDHC2_DATA3, 
                        IOMUXC_SW_PAD_CTL_PAD_SRE_MASK |
                        IOMUXC_SW_PAD_CTL_PAD_DSE(1U) |
                        IOMUXC_SW_PAD_CTL_PAD_SPEED(2U) |
                        IOMUXC_SW_PAD_CTL_PAD_PKE_MASK |
                        IOMUXC_SW_PAD_CTL_PAD_PUE_MASK |
                        IOMUXC_SW_PAD_CTL_PAD_PUS(1U) |
                        IOMUXC_SW_PAD_CTL_PAD_HYS_MASK);
}

void bsp_emmc_init(void)
{
    mmc_card_t *card = &g_mmc; 
    emmc_pinmux_init();   
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
    uart_printf("MMC READ:%s\r\n",readData);
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
