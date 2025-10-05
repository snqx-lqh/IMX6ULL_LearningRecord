#include "bsp_delay.h"

/* 假设 ipg_clk = 66MHz，我们要分频成 1MHz (1us精度) */
void delay_init(void)
{
    gpt_config_t gptConfig;

    GPT_GetDefaultConfig(&gptConfig);       // 获取默认配置
    gptConfig.clockSource = kGPT_ClockSource_Periph; 
    gptConfig.divider = 66;                 // 66分频 (66MHz / 66 = 1MHz)

    GPT_Init(GPT1, &gptConfig);             // 初始化 GPT1
    GPT_SetClockDivider(GPT1, gptConfig.divider);
    GPT_StartTimer(GPT1);                   // 启动定时器
}

/* us 延时 */
void delayus(unsigned    int usdelay)
{
	unsigned long oldcnt,newcnt;
	unsigned long tcntvalue = 0;	/* 走过的总时间  */

	oldcnt = GPT_GetCurrentTimerCount(GPT1);
	while(1)
	{
		newcnt = GPT_GetCurrentTimerCount(GPT1);
		if(newcnt != oldcnt)
		{
			if(newcnt > oldcnt)		/* GPT是向上计数器,并且没有溢出 */
				tcntvalue += newcnt - oldcnt;
			else  					/* 发生溢出    */
				tcntvalue += 0XFFFFFFFF-oldcnt + newcnt;
			oldcnt = newcnt;
			if(tcntvalue >= usdelay)/* 延时时间到了 */
			break;			 		/*  跳出 */
		}
	}
}

/* ms 延时 */
void delayms(uint32_t ms)
{
    while (ms--)
    {
        delayus(1000);
    }
}