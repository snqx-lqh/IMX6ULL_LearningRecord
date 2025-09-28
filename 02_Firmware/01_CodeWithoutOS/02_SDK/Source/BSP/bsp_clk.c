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
	/* 1、设置ARM内核时钟为792MHz */
	/* Boot ROM did initialize the XTAL, here we only sets external XTAL OSC freq */
    CLOCK_SetXtalFreq(24000000U);
    CLOCK_SetRtcXtalFreq(32768U);

    /* Switch CPU off ARM PLL */
    if (CLOCK_GetMux(kCLOCK_Pll1SwMux) == 0) /* CPU runs on ARM PLL */
    {
        CLOCK_SetMux(kCLOCK_StepMux, 0);   /* Set Step MUX to OSC */
        CLOCK_SetMux(kCLOCK_Pll1SwMux, 1); /* Let CPU run on Step MUX */
    }

	const clock_arm_pll_config_t g_ccmConfigArmPll = {.loopDivider = 66U};
    CLOCK_InitArmPll(&g_ccmConfigArmPll); /* Configure ARM PLL to 1056M */
    CLOCK_SetDiv(kCLOCK_ArmDiv,0);
	CLOCK_SetMux(kCLOCK_Pll1SwMux, 0);    /* Now CPU runs again on ARM PLL at 528M (with divider 2) */

	
	/* 2、设置PLL2(SYS PLL)各个PFD */
	reg = CCM_ANALOG->PFD_528;
	reg &= ~(0X3F3F3F3F);		/* 清除原来的设置 						*/
	reg |= 32<<24;				/* PLL2_PFD3=528*18/32=297Mhz 	*/
	reg |= 24<<16;				/* PLL2_PFD2=528*18/24=396Mhz(DDR使用的时钟，最大400Mhz) */
	reg |= 16<<8;				/* PLL2_PFD1=528*18/16=594Mhz 	*/
	reg |= 27<<0;				/* PLL2_PFD0=528*18/27=352Mhz  	*/
	CCM_ANALOG->PFD_528=reg;	/* 设置PLL2_PFD0~3 		 		*/

	/* 3、设置PLL3(USB1)各个PFD */
	reg = 0;					/* 清零   */
	reg = CCM_ANALOG->PFD_480;
	reg &= ~(0X3F3F3F3F);		/* 清除原来的设置 							*/
	reg |= 19<<24;				/* PLL3_PFD3=480*18/19=454.74Mhz 	*/
	reg |= 17<<16;				/* PLL3_PFD2=480*18/17=508.24Mhz 	*/
	reg |= 16<<8;				/* PLL3_PFD1=480*18/16=540Mhz		*/
	reg |= 12<<0;				/* PLL3_PFD0=480*18/12=720Mhz	 	*/
	CCM_ANALOG->PFD_480=reg;	/* 设置PLL3_PFD0~3 					*/	

	/* 4、设置AHB时钟 最小6Mhz， 最大132Mhz (boot rom自动设置好了可以不用设置)*/
	CCM->CBCMR &= ~(3 << 18); 	/* 清除设置*/ 
	CCM->CBCMR |= (1 << 18);	/* pre_periph_clk=PLL2_PFD2=396MHz */
	CCM->CBCDR &= ~(1 << 25);	/* periph_clk=pre_periph_clk=396MHz */
	while(CCM->CDHIPR & (1 << 5));/* 等待握手完成 */
		
	/* 修改AHB_PODF位的时候需要先禁止AHB_CLK_ROOT的输出，但是
	 * 我没有找到关闭AHB_CLK_ROOT输出的的寄存器，所以就没法设置。
	 * 下面设置AHB_PODF的代码仅供学习参考不能直接拿来使用！！
	 * 内部boot rom将AHB_PODF设置为了3分频，即使我们不设置AHB_PODF，
	 * AHB_ROOT_CLK也依旧等于396/3=132Mhz。
	 */
#if 0
	/* 要先关闭AHB_ROOT_CLK输出，否则时钟设置会出错 */
	CCM->CBCDR &= ~(7 << 10);	/* CBCDR的AHB_PODF清零 */
	CCM->CBCDR |= 2 << 10;		/* AHB_PODF 3分频，AHB_CLK_ROOT=132MHz */
	while(CCM->CDHIPR & (1 << 1));/
* 等待握手完成 */
#endif
	
	/* 5、设置IPG_CLK_ROOT最小3Mhz，最大66Mhz (boot rom自动设置好了可以不用设置)*/
	CCM->CBCDR &= ~(3 << 8);	/* CBCDR的IPG_PODF清零 */
	CCM->CBCDR |= 1 << 8;		/* IPG_PODF 2分频，IPG_CLK_ROOT=66MHz */
	
	/* 6、设置PERCLK_CLK_ROOT时钟 */
	CCM->CSCMR1 &= ~(1 << 6);	/* PERCLK_CLK_ROOT时钟源为IPG */
	CCM->CSCMR1 &= ~(7 << 0);	/* PERCLK_PODF位清零，即1分频 */
	

	clk_enable();
}