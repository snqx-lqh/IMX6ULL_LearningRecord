#include "bsp_emmc.h"
#include "bsp_uart.h"
 

status_t eMMC_SetClock(USDHC_Type *base, uint32_t srcClock_Hz, uint32_t targetClock_Hz);
#define OCR_VOLTAGE_MASK 0x00FF8000U  // 电压范围掩码（参考eMMC规格书）
void bsp_emmc_init(void)
{
 

}
 
/* 全局句柄和配置结构体 */
static usdhc_handle_t g_usdhcHandle;
static usdhc_adma_config_t g_admaConfig;
static uint32_t g_admaTable[32] __attribute__((aligned(16))); /* ADMA描述符表 */

/* 回调函数实现 */
static void USDHC_TransferCompleteCallback(USDHC_Type *base, usdhc_handle_t *handle, status_t status, void *userData)
{
    /* 传输完成回调处理 */
    if (status != kStatus_Success)
    {
        // 处理传输错误
    }
}

static void USDHC_CardInsertedCallback(void)
{
    // 卡插入处理
}

static void USDHC_CardRemovedCallback(void)
{
    // 卡移除处理
}

// 应答类型枚举（参考1.1.6）
typedef enum {
    RESP_NONE = 0,
    RESP_R1,    // 48位应答（含设备状态）
    RESP_R2,    // 136位应答（CID/CSD）
    RESP_R3     // 48位应答（OCR）
} RespType;

// 命令结构体（封装命令参数与应答）
typedef struct {
    uint8_t  cmd_idx;        // 命令编号（如CMD0=0, CMD1=1）
    uint32_t cmd_arg;        // 命令参数
    RespType resp_type;      // 应答类型
    uint32_t resp[4];        // 应答缓存（R2需4个32位）
} EMMC_Cmd;

 typedef enum 
{
 	ResponseTypeNone = 0U,
 	ResponseTypeR1   = 1U,
 	ResponseTypeR1b  = 2U,
 	ResponseTypeR2   = 3U,
 	ResponseTypeR3   = 4U,
 	ResponseTypeR4	 = 5U,
 	ResponseTypeR5   = 6U,
 	ResponseTypeR5b  = 7U,
 	ResponseTypeR6   = 8U,
 	ResponseTypeR7   = 9U
}USDHC_Respones_Type;
 
typedef struct
{
 	unsigned int  index;
 	unsigned int  arg;
 	unsigned int  response[4];
 	USDHC_Respones_Type response_type;
 	unsigned int  mix_ctrl;
 	unsigned int  xfr_typ;
}USDHC_Command;



void Emmc_PinConfig(void) {
// 配置引脚复用
	    IOMUXC_SetPinMux(IOMUXC_NAND_WE_B_USDHC2_CMD, 0U);
	    IOMUXC_SetPinConfig(IOMUXC_NAND_WE_B_USDHC2_CMD, 0x17059);
		IOMUXC_SetPinMux(IOMUXC_NAND_RE_B_USDHC2_CLK, 0U);
	    IOMUXC_SetPinConfig(IOMUXC_NAND_RE_B_USDHC2_CLK, 0x17059);
		IOMUXC_SetPinMux(IOMUXC_NAND_ALE_USDHC2_RESET_B, 0U);
	    IOMUXC_SetPinConfig(IOMUXC_NAND_ALE_USDHC2_RESET_B, 0x17059);
		IOMUXC_SetPinMux(IOMUXC_NAND_DATA00_USDHC2_DATA0, 0U);
	    IOMUXC_SetPinConfig(IOMUXC_NAND_DATA00_USDHC2_DATA0, 0x17059);
		IOMUXC_SetPinMux(IOMUXC_NAND_DATA01_USDHC2_DATA1, 0U);
	    IOMUXC_SetPinConfig(IOMUXC_NAND_DATA01_USDHC2_DATA1, 0x17059);
		IOMUXC_SetPinMux(IOMUXC_NAND_DATA02_USDHC2_DATA2, 0U);
	    IOMUXC_SetPinConfig(IOMUXC_NAND_DATA02_USDHC2_DATA2, 0x17059);
		IOMUXC_SetPinMux(IOMUXC_NAND_DATA03_USDHC2_DATA3, 0U);
	    IOMUXC_SetPinConfig(IOMUXC_NAND_DATA03_USDHC2_DATA3, 0x17059);
		IOMUXC_SetPinMux(IOMUXC_NAND_DATA04_USDHC2_DATA4, 0U);
	    IOMUXC_SetPinConfig(IOMUXC_NAND_DATA04_USDHC2_DATA4, 0x17059);
		IOMUXC_SetPinMux(IOMUXC_NAND_DATA05_USDHC2_DATA5, 0U);
	    IOMUXC_SetPinConfig(IOMUXC_NAND_DATA05_USDHC2_DATA5, 0x17059);
		IOMUXC_SetPinMux(IOMUXC_NAND_DATA06_USDHC2_DATA6, 0U);
	    IOMUXC_SetPinConfig(IOMUXC_NAND_DATA06_USDHC2_DATA6, 0x17059);
		IOMUXC_SetPinMux(IOMUXC_NAND_DATA07_USDHC2_DATA7, 0U);
	    IOMUXC_SetPinConfig(IOMUXC_NAND_DATA07_USDHC2_DATA7, 0x17059);
}

void Emmc_ClockConfig(void) {
    // ① 使能USDHC2时钟（CCGR6[CG2] = 0b11，参考1.3.2.2）
    //*CCM_CCGR6 = (*CCM_CCGR6 & ~(0x3U << 4)) | (0x3U << 4);
	
	

    // ② 配置时钟分频：USDHC2_CLK = PLL2_PFD2/(SDCLKFS * DVS)
    // PLL2_PFD2 = 396MHz → 分频后目标400KHz，计算得SDCLKFS=128（0x40）、DVS=4（0x3）
    //usdhc2_reg->SYS_CTRL &= ~((0xFFU << 8) | (0x0FU << 4));  // 清除旧分频值
    //usdhc2_reg->SYS_CTRL |= (0x40U << 8) | (0x03U << 4);     // 新分频值：128*4=512

    // ③ 等待时钟稳定（PRES_STATE[SDSTB] = 1，参考1.3.2.2）
    //while (!(usdhc2_reg->PRES_STATE & 0x8U));


	CCM->CCGR6 = (CCM->CCGR6 & ~(0x3U << 4)) | (0x3U << 4);
	USDHC2->SYS_CTRL &= ~((0xFFU << 8) | (0x0FU << 4));  // 清除旧分频值
	USDHC2->SYS_CTRL |= (0x40U << 8) | (0x03U << 4);     // 新分频值：128*4=512
	while(!(USDHC2->PRES_STATE & 0x8U));
}

// 延时函数（单位：微秒，需根据实际主频实现）
static void Reset_delay_us(uint32_t us) {
    // 示例：假设主频为 528MHz，1 微秒约 528 个指令周期
    for (uint32_t i = 0; i < us * 528; i++) {
        __asm__("nop");
    }
}

static void Reset_delay_ms(uint32_t ms) {
    Reset_delay_us(ms*1000);
}

// 优化后的软复位函数（返回 0 成功）
uint8_t USDHC2_Reset(void) {
    uint32_t timeout_us = 10000;  // 最长等待 10ms
    uint32_t start_us = 0;        // 需配合定时器获取当前时间
    
    // 触发软复位
    USDHC2->SYS_CTRL |= USDHC_SYS_CTRL_RSTA_MASK;
    
    // 等待复位完成（RSTA 位清零），最多等待 10ms
    while ((USDHC2->SYS_CTRL & USDHC_SYS_CTRL_RSTA_MASK)) {
        Reset_delay_us(10);  // 每 10us 检查一次
        timeout_us -= 10;
        if (timeout_us == 0) {
            uart_printf("USDHC Reset Timeout\n");
            return 1;  // 超时失败
        }
    }
	uart_printf("USDHC Reset Success\n");
    return 0;
}

void USDHC_ReadResponse(USDHC_Command *command);

// 发送eMMC命令（核心交互函数，参考1.3.4.1）
uint8_t USDHC_SendCmd(USDHC_Command *cmd) {
    // ① 等待命令/数据空闲（CIHB=0、CDIHB=0）
    //while ((USDHC2->PRES_STATE & (USDHC_PRES_STATE_CIHB_MASK | USDHC_PRES_STATE_CDIHB_MASK)));
	
	/* 等待命令发送完成 */
  	// CIHB掩码：0x01
	uart_printf("MMC %d 1 \n\r",cmd->index);
 	while( USDHC2->PRES_STATE & 0x01U )
  	{
 	}
	uart_printf("MMC %d 2 \n\r",cmd->index);
	/* 等待数据完成 */
	// CDIHB掩码：0x02
	while( USDHC2->PRES_STATE & 0x02U )
	{
 	}

	//​ 确认可以发送CMD时，根据命令的类型配置uSDHC2_MIX_CTRL的DDR_EN(DDR模式)、
	//DTDSEL(数据传输方向，1为读操作，0为写操作)、BCEN（块计数使能）和MSBSEL（单块或多块操作，0为单个块操作，1为多个块操作）位。
	/* 配置传输方式 */
	// BCEN掩码: 0x02U
 	// DDR_EN掩码：0x08U
 	// DTDSEL掩码：0x10U
 	// MSBSEL掩码：0x20U
 	USDHC2->MIX_CTRL &= ~(0x20U | 0x2U | 0x10U | 0x8U );
	USDHC2->MIX_CTRL |= ((cmd->mix_ctrl) & (0x20U | 0x2U | 0x10U | 0x8U ));	

    // ② 配置命令参数与传输类型
    USDHC2->CMD_ARG = cmd->arg;  // 写入命令参数
    // CMD_XFR_TYP配置：命令索引（24~29位）+ 应答类型（16~17位）+ CRC使能
	/* 配置命令参数 */
 	// CDMINX掩码：0x3F000000U
 	// DPSEL掩码：0x200000U
 	// CICEN掩码：0x100000U
 	// CCCEN掩码：0x80000U
 	// PSPTYP[1:0]掩码：0x30000U
    USDHC2->CMD_XFR_TYP &= ~(0x3F000000U | 0x200000U | 0x100000U | 0x80000U | 0x30000U);
    USDHC2->CMD_XFR_TYP = (((cmd->index << 24U) & 0x3F000000U) | 
							((cmd->xfr_typ) &(0x3F000000U | 0x200000U | 0x100000U | 0x80000U | 0x30000U))); 
	
	uart_printf("MMC %d 3 \n\r",cmd->index);
	// CC位掩码：0x01
	while( !(USDHC2->INT_STATUS & 0x1U ) )
	{

	}
	uart_printf("MMC %d 4 \n\r",cmd->index);
	/* 清中断标记位 */
	// CC位掩码：0x01
	USDHC2->INT_STATUS &= 0x01U ;

	USDHC_ReadResponse(cmd);
    return 0;
}

void USDHC_ReadResponse(USDHC_Command *command)
{
	unsigned int i;
	
	if( command->response_type != ResponseTypeNone )
	{
		command->response[0] = USDHC2->CMD_RSP0;
		
		if( command->response_type == ResponseTypeR2 )
		{
			command->response[1] = USDHC2->CMD_RSP1;
			command->response[2] = USDHC2->CMD_RSP2;
			command->response[3] = USDHC2->CMD_RSP3;

			i = 4;
		
			do
			{
				command->response[i - 1] <<= 8;
				if (i > 1)
				{
					command->response[i - 1] |= ((command->response[i - 2] & 0xFF000000U) >> 24U);
				}
			} while (i--);
		}
	}
}
 

// eMMC初始化（返回0成功，1失败）
uint8_t Emmc_Init(void) {
    EMMC_Cmd cmd;
	USDHC_Command command;
	int i = 0;
    // ① 引脚与时钟配置
    Emmc_PinConfig();
    Emmc_ClockConfig();

    // ② 软复位USDHC2
    if (USDHC2_Reset() != 0) {
        uart_printf("USDHC2 Reset Fail\n");
        return 1;
    }

    // ③ eMMC初始化流程（参考1.1.9）
    // 1. CMD0：进入空闲态（参数0x00000000，无应答）
    //cmd.cmd_idx = 0;
    //cmd.cmd_arg = 0x00000000;
    //cmd.resp_type = RESP_NONE;
    //USDHC_SendCmd(&cmd);
    command.index = 0;
	command.arg = 0;
	command.mix_ctrl = 0;
	command.xfr_typ = 0;
	command.response_type = ResponseTypeNone;
	//发送CMD0
	USDHC_SendCmd(&command);

    // 2. CMD1：读取OCR寄存器（检查电压匹配与复位完成，R3应答）
 //   cmd.cmd_idx = 1;
 //   cmd.cmd_arg = 0x00000000;
 //   cmd.resp_type = RESP_R3;
 //   USDHC_SendCmd(&cmd);
 //   // 增加多次重试机制
	//uint8_t retry = 5;
	//while (retry--) {
	//    if ((cmd.resp[0] & 0x80000000U) != 0) {
	//        break;  // 复位完成，退出循环
	//    }
	//	uart_printf("eMMC Reset Not Ready retry:%d\n",retry);
	//    Reset_delay_ms(50);  // 等待10ms后重试
	//    USDHC_SendCmd(&cmd);
	//}
	uart_printf("eMMC Reset  Ready\n");

	//Reset_delay_ms(20);  // 等待20ms，确保eMMC进入稳定空闲态

	for( i = 0; i < 10; i++ )
	{
		//发送CMD1
		//设置响应类型 R3
		//答复长度为48bit，RSPTYP[1:0] = 0b10 = 2
		command.index = 1;
		command.arg = 0;
		command.mix_ctrl = 0;
		command.xfr_typ = 2 << 16;
		command.response_type = ResponseTypeR3;
	
		USDHC_SendCmd(&command);
	
		// 等待EMMC复位完成
		// 在OCR中第31为表示EMMC设备是否准备好处理数据，1为准备好，0为忙
		if ( command.response[0] & (1U << 31U) )
		{
			uart_printf("MMC is Ready ok \n\r");
			break;
		}
		else
		{
			uart_printf("MMC is Busy.\n\r");
		}
	}

    // 3. CMD2：读取CID寄存器（设备唯一标识，R2应答）
 //   cmd.cmd_idx = 2;
 //   cmd.cmd_arg = 0x00000000;
 //   cmd.resp_type = RESP_R2;
 //   USDHC_SendCmd(&cmd);
	//uart_printf("eMMC CID  Ready\n");
	//发送CMD2
	//设置响应类型 R2
	//答复长度为136bit，RSPTYP[1:0] = 0b01 = 1
	command.index = 2;
	command.arg = 0;
	command.mix_ctrl = 0;
	command.xfr_typ = 1 << 16 | 1 << 19;
	command.response_type = ResponseTypeR2;
	
	USDHC_SendCmd(&command);
	uart_printf("ManufacturerID: 0x%x \n\r", (unsigned char)((command.response[3U] & 0xFF000000U) >> 24U));
	uart_printf("CBX: 0x%x \n\r", (unsigned char)((command.response[3U] & 0x00FF0000U) >> 16U));
	uart_printf("ApplicationID: 0x%x \n\r", (unsigned char)((command.response[3U] & 0x0000FF00U) >> 8U));
	uart_printf("Product version: 0x%x \n\r", (unsigned char)((command.response[1U] & 0xFF000000U) >> 24U));



    // 4. CMD3：分配RCA（相对地址，R1应答）
    //cmd.cmd_idx = 3;
    //cmd.cmd_arg = 0x00000000;
    //cmd.resp_type = RESP_R1;
    //USDHC_SendCmd(&cmd);
    //if ((cmd.resp[0] & 0xFFFF0000U) == 0) {  // RCA占应答[39~24]位，非0表示分配成功
    //    uart_printf("RCA Assign Fail\n");
    //    return 1;
    //}
	// Send CMD3 设置设备相对地址
	command.index = 3;
	command.arg = 1 << 16;
	command.mix_ctrl = 0;
	command.xfr_typ = 2 << 16 | 1 << 19 | 1 << 20;
	command.response_type = ResponseTypeR1; 
	USDHC_SendCmd(&command);
	// COM_CRC_ERROR:(1U << 23U)
	// ILLEGAL_COMMAND:(1U << 22U)
	if (!(command.response[0] & ((1U << 23U) | (1U << 22U))))
	{
		uart_printf("Emmc in Stand-by State\n\r");
	}
	else
	{
		uart_printf("SDMMC_R1 Error\n\r");
		return 0;
	}
	// Send CMD9 读取CSD信息
	command.index = 9;
	command.arg = 1 << 16;
	command.mix_ctrl = 0;
	command.xfr_typ = 1 << 16 | 1 << 19;
	command.response_type = ResponseTypeR2;

	USDHC_SendCmd(&command);

	uart_printf("CSD structure: %x \n\r", (unsigned char)((command.response[3U] & 0xC0000000U) >> 30U));
 

    uart_printf("eMMC Init Success\n");
    return 0;
}

/* 初始化USDHC控制器 */
status_t eMMC_Init(USDHC_Type *base)
{

	Emmc_ClockConfig();
	
	Emmc_ClockConfig();

    usdhc_config_t config;
    
    /* 初始化配置结构体 */
    config.endianMode = kUSDHC_EndianModeLittle;
    config.writeWatermarkLevel = 32;  // 根据需求调整
    config.readWatermarkLevel = 32;   // 根据需求调整
    config.dataTimeout = 0xFFFFU;     // 超时值
     
    /* 初始化控制器 */
    USDHC_Init(base, &config);
    
	/* 配置8位数据总线（关键补充） */
    uint32_t protCtrl = base->PROT_CTRL;
    protCtrl &= ~USDHC_PROT_CTRL_DTW_MASK;
    protCtrl |= USDHC_PROT_CTRL_DTW(kUSDHC_DataBusWidth8Bit); // 8位总线
    base->PROT_CTRL = protCtrl;
	uart_printf("2\r\n");
    /* 初始化句柄 */
    USDHC_TransferCreateHandle(base, &g_usdhcHandle, USDHC_TransferCompleteCallback, NULL);
    
    /* 配置回调函数 */
    g_usdhcHandle.callback.CardInserted = USDHC_CardInsertedCallback;
    g_usdhcHandle.callback.CardRemoved = USDHC_CardRemovedCallback;
    
    /* 配置ADMA */
    g_admaConfig.dmaMode = kUSDHC_DmaModeAdma2;
    g_admaConfig.admaTable = g_admaTable;
    g_admaConfig.admaTableWords = ARRAY_SIZE(g_admaTable);
    g_admaConfig.burstLen = 8;  // 突发长度
    
	uart_printf("3\r\n");
    /* 等待卡稳定 */
    if (!USDHC_SetCardActive(base, 1000U))
    {
        return kStatus_Fail;
    }
    uart_printf("4\r\n");
    return kStatus_Success;
}

/* eMMC块读取函数（阻塞方式） */
status_t eMMC_ReadBlocks(USDHC_Type *base, 
                        uint32_t startBlock, 
                        uint8_t *buffer, 
                        uint32_t blockCount, 
                        uint32_t blockSize)
{
    usdhc_transfer_t transfer;
    usdhc_command_t command;
    usdhc_data_t data;
    
    /* 初始化命令结构体 */
    memset(&command, 0, sizeof(usdhc_command_t));
    command.index = 18;  // CMD18 - 多块读取
    command.argument = startBlock * blockSize;  // 起始地址
    command.responseType = kCARD_ResponseTypeR1;
    
    /* 初始化数据结构体 */
    memset(&data, 0, sizeof(usdhc_data_t));
    data.blockSize = blockSize;
    data.blockCount = blockCount;
    data.rxData = (uint32_t *)buffer;  // 接收缓冲区
    data.enableAutoCommand12 = true;   // 启用自动CMD12停止传输
    
    /* 初始化传输结构体 */
    memset(&transfer, 0, sizeof(usdhc_transfer_t));
    transfer.command = &command;
    transfer.data = &data;
    
    /* 执行阻塞读取 */
    return USDHC_TransferBlocking(base, &g_admaConfig, &transfer);
}

/* eMMC块写入函数（阻塞方式） */
status_t eMMC_WriteBlocks(USDHC_Type *base, 
                         uint32_t startBlock, 
                         const uint8_t *buffer, 
                         uint32_t blockCount, 
                         uint32_t blockSize)
{
    usdhc_transfer_t transfer;
    usdhc_command_t command;
    usdhc_data_t data;
    
    /* 初始化命令结构体 */
    memset(&command, 0, sizeof(usdhc_command_t));
    command.index = 25;  // CMD25 - 多块写入
    command.argument = startBlock * blockSize;  // 起始地址
    command.responseType = kCARD_ResponseTypeR1;
    
    /* 初始化数据结构体 */
    memset(&data, 0, sizeof(usdhc_data_t));
    data.blockSize = blockSize;
    data.blockCount = blockCount;
    data.txData = (const uint32_t *)buffer;  // 发送缓冲区
    data.enableAutoCommand12 = true;         // 启用自动CMD12停止传输
    data.enableAutoCommand23 = true;         // 启用自动CMD23设置块数
    
    /* 初始化传输结构体 */
    memset(&transfer, 0, sizeof(usdhc_transfer_t));
    transfer.command = &command;
    transfer.data = &data;
    
    /* 执行阻塞写入 */
    return USDHC_TransferBlocking(base, &g_admaConfig, &transfer);
	//eMMC_SetClock(USDHC_BASE, 240000000U, 52000000U); 
}

/* eMMC单块读取函数（非阻塞方式） */
status_t eMMC_ReadBlocksNonBlocking(USDHC_Type *base, 
                                   uint32_t startBlock, 
                                   uint8_t *buffer, 
                                   uint32_t blockCount, 
                                   uint32_t blockSize)
{
    usdhc_transfer_t transfer;
    usdhc_command_t command;
    usdhc_data_t data;
    
    /* 检查是否正在传输 */
    if (USDHC_GetPresentStatusFlags(base) & (kUSDHC_CommandInhibitFlag | kUSDHC_DataInhibitFlag))
    {
        return kStatus_USDHC_BusyTransferring;
    }
    
    /* 初始化命令结构体 */
    memset(&command, 0, sizeof(usdhc_command_t));
    command.index = 18;  // CMD18 - 多块读取
    command.argument = startBlock * blockSize;
    command.responseType = kCARD_ResponseTypeR1;
    
    /* 初始化数据结构体 */
    memset(&data, 0, sizeof(usdhc_data_t));
    data.blockSize = blockSize;
    data.blockCount = blockCount;
    data.rxData = (uint32_t *)buffer;
    data.enableAutoCommand12 = true;
    
    /* 初始化传输结构体 */
    memset(&transfer, 0, sizeof(usdhc_transfer_t));
    transfer.command = &command;
    transfer.data = &data;
    
    /* 执行非阻塞读取 */
    return USDHC_TransferNonBlocking(base, &g_usdhcHandle, &g_admaConfig, &transfer);
}

/* eMMC单块写入函数（非阻塞方式） */
status_t eMMC_WriteBlocksNonBlocking(USDHC_Type *base, 
                                    uint32_t startBlock, 
                                    const uint8_t *buffer, 
                                    uint32_t blockCount, 
                                    uint32_t blockSize)
{
    usdhc_transfer_t transfer;
    usdhc_command_t command;
    usdhc_data_t data;
    
    /* 检查是否正在传输 */
    if (USDHC_GetPresentStatusFlags(base) & (kUSDHC_CommandInhibitFlag | kUSDHC_DataInhibitFlag))
    {
        return kStatus_USDHC_BusyTransferring;
    }
    
    /* 初始化命令结构体 */
    memset(&command, 0, sizeof(usdhc_command_t));
    command.index = 25;  // CMD25 - 多块写入
    command.argument = startBlock * blockSize;
    command.responseType = kCARD_ResponseTypeR1;
    
    /* 初始化数据结构体 */
    memset(&data, 0, sizeof(usdhc_data_t));
    data.blockSize = blockSize;
    data.blockCount = blockCount;
    data.txData = (const uint32_t *)buffer;
    data.enableAutoCommand12 = true;
    data.enableAutoCommand23 = true;
    
    /* 初始化传输结构体 */
    memset(&transfer, 0, sizeof(usdhc_transfer_t));
    transfer.command = &command;
    transfer.data = &data;
    
    /* 执行非阻塞写入 */
    return USDHC_TransferNonBlocking(base, &g_usdhcHandle, &g_admaConfig, &transfer);
}

/* 设置eMMC时钟频率 */
status_t eMMC_SetClock(USDHC_Type *base, uint32_t srcClock_Hz, uint32_t targetClock_Hz)
{
    uint32_t actualClock;
    
    actualClock = USDHC_SetSdClock(base, srcClock_Hz, targetClock_Hz);
    if (actualClock == 0)
    {
        return kStatus_Fail;
    }
    
    return kStatus_Success;
}
