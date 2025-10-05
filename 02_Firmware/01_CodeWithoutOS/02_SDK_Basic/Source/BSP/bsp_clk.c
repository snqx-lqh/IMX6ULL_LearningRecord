#include "bsp_clk.h"

void clk_enable(void)
{
	CCM->CCGR0 = 0XFFFFFFFF;
	CCM->CCGR1 = 0XFFFFFFFF;
	CCM->CCGR2 = 0XFFFFFFFF;
	CCM->CCGR3 = 0XFFFFFFFF;
	CCM->CCGR4 = 0XFFFFFFFF;
	CCM->CCGR5 = 0XFFFFFFFF;
	CCM->CCGR6 = 0XFFFFFFFF;
}

void bsp_system_clk_init(void)
{
	unsigned int reg = 0;
	// 设置系统相关时钟的默认参数
    CLOCK_SetXtalFreq(24000000U);
    CLOCK_SetRtcXtalFreq(32768U);

    /* 1、设置 ARM 内核时钟为 528MHz */
    /* 1.1、判断当使用哪个时钟源启动的，正常情况下是由 pll1_sw_clk 驱动的，而
     * pll1_sw_clk 有两个来源：pll1_main_clk 和 step_clk，如果要
     * 让 I.MX6ULL 跑到 528M，那必须选择 pll1_main_clk 作为 pll1 的时钟
     * 源。如果我们要修改 pll1_main_clk 时钟的话就必须先将 pll1_sw_clk 从
     * pll1_main_clk 切换到 step_clk,当修改完以后再将 pll1_sw_clk 切换
     * 回 pll1_main_clk，step_clk 等于 24MHz。
     */
    if (CLOCK_GetMux(kCLOCK_Pll1SwMux) == 0) /* CPU 工作在 pll1_main_clk */
    {
        CLOCK_SetMux(kCLOCK_StepMux, 0);     /* 配置 step_clk 时钟源为 24MHz OSC */
        CLOCK_SetMux(kCLOCK_Pll1SwMux, 1);   /* 配置 pll1_sw_clk 时钟源为 step_clk */
    }
    /* 1.2、设置 pll1_main_clk 为 1056MHz,也就是 528*2=1056MHZ,
     * 由公式：Fout = Fin * div_select / 2.0，
     * 1056=24*div_select/2.0, 得出：div_select=88。 
     */
	const clock_arm_pll_config_t g_ccmConfigArmPll = {.loopDivider = 88U};
    /* Configure ARM PLL to 1056M */
    CLOCK_InitArmPll(&g_ccmConfigArmPll); 
    /* 配置二分频 0-7 分别对应 1-8 分频 1056MHZ/2 = 528MHz */
    CLOCK_SetDiv(kCLOCK_ArmDiv,1);
    /* 将 pll_sw_clk 时钟切换回 pll1_main_clk */
	CLOCK_SetMux(kCLOCK_Pll1SwMux, 0);     
    
    // 使能时钟
	clk_enable();
}