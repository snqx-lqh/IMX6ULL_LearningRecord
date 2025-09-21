#include "bsp_led.h"

int main(void) {
	clk_enable();		/* 使能所有的时钟		 	*/
	led_init();			/* 初始化led 			*/

	while(1)			/* 死循环 				*/
	{	
		led_off();		/* 关闭LED   			*/
		delay(50000);		/* 延时大约500ms 		*/

		led_on();		/* 打开LED		 	*/
		delay(50000);		/* 延时大约500ms 		*/
	}

	return 0;
}

/*************************** End of file ****************************/
