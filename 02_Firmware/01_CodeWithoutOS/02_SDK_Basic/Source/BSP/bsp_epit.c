#include "bsp_epit.h"

 
/* 获得EPIT的时钟 */
#define GET_EPIT_CLK_FREQ     (CLOCK_GetFreq(kCLOCK_IpgClk) / \
                              (CLOCK_GetDiv(kCLOCK_PerclkDiv) + 1U))

 
__attribute__((weak)) void EPIT1_IRQHander_Callback(void){}

void EPIT1_IRQHander(void)
{
    // 清除中断标志位
    EPIT_ClearStatusFlags(EPIT1, kEPIT_OutputCompareFlag);
    EPIT1_IRQHander_Callback(); 
}


void bsp_epit1_init(uint32_t us)
{
    epit_config_t epitConfig;
	// 注册中断服务函数
    SystemInstallIrqHandler(EPIT1_IRQn, (system_irq_handler_t)(uint32_t)EPIT1_IRQHander, NULL);
    // 初始化 EPIT 配置
    EPIT_GetDefaultConfig(&epitConfig);
    // 初始化 EPIT
    EPIT_Init(EPIT1, &epitConfig);
    // 设置定时器分频 Count是us
    EPIT_SetTimerPeriod(EPIT1, USEC_TO_COUNT(us, GET_EPIT_CLK_FREQ) - 1);
    EPIT_SetOutputCompareValue(EPIT1, 0);
    // 使能输出比较中断
    EPIT_EnableInterrupts(EPIT1, kEPIT_OutputCompareInterruptEnable);
    // 使能中断
    EnableIRQ(EPIT1_IRQn);
    // 开始定时器
    EPIT_StartTimer(EPIT1);
}