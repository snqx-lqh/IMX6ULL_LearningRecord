#include "bsp_clk.h"
#include "bsp_led.h"
#include "bsp_key.h"
#include "bsp_uart.h"
#include "bsp_exti.h"
#include "bsp_beep.h"
#include "bsp_epit.h"
#include "bsp_rtc.h"
#include "bsp_delay.h"
#include "bsp_i2c.h"
#include "bsp_spi.h"

#define I2C_MPU6050_DEMO 0
#define SPI_W25Q128_DEMO 0

#if I2C_MPU6050_DEMO
#include "mpu6050.h"
int16_t gx,gy,gz;
#endif

#if SPI_W25Q128_DEMO
#include "w25q128.h"
//要写入到W25Q64的字符串数组
const uint8_t TEXT_Buffer[]={"ELITE STM32 SPI TEST"};
#define SIZE sizeof(TEXT_Buffer)
uint16_t w25_id = 0;
uint8_t datatemp[SIZE];
uint32_t FLASH_SIZE; 
#endif

int main(void) {
    unsigned char a=0;
    uint8_t  led_state = 0;
	int ret;
    
    GIC_Init();
    SystemInitIrqTable();             //初始化中断向量表
	__set_VBAR((uint32_t)0x87800000); // 中断向量表偏移，偏移到起始地址   
	
	bsp_system_clk_init();
    delay_init();
    bsp_led_init();
	bsp_key_init();
    // 初始化 UART
    bsp_uart1_init(115200);
	bsp_exti_init();
	bsp_beep_init();
	bsp_epit1_init(1000000);
	bsp_rtc_init();
    bsp_i2c1_master_init();
	bsp_spi_init();

#if I2C_MPU6050_DEMO
    bsp_i2c1_master_init();
    ret = mpu6050_init();
    if(ret == 0)
    {
        uart_printf("mpu init sucess\r\n");
    }else{
        uart_printf("mpu init failed\r\n");
    }
#endif

#if SPI_W25Q128_DEMO
    
    while(1)
	{
		w25_id  = W25QXX_ReadID();
        if (w25_id == W25Q128 || w25_id == NM25Q128)
        {
            uart_printf("w25_id is W25Q128\r\n");
            break;
        }else{
            uart_printf("w25 init failed,w25_id is %x\r\n",w25_id);
        }
	}
    FLASH_SIZE=128*1024*1024;
    W25QXX_Write((uint8_t*)TEXT_Buffer,FLASH_SIZE-100,SIZE);			//从倒数第100个地址处开始,写入SIZE长度的数据
    uart_printf("write sucess\r\n");
    W25QXX_Read(datatemp,FLASH_SIZE-100,SIZE);					//从倒数第100个地址处开始,读出SIZE个字节
    uart_printf("read string:%s\r\n",datatemp);
#endif

    uart_printf("all init\r\n");
    
    while (1)
    {
#if I2C_MPU6050_DEMO
        mpu6050_get_gyro(&gx,&gy,&gz);
        uart_printf("mpu6050 gx:%d gy%d gz:%d\r\n",gx,gy,gz);
#endif
        uart_printf("CPU is RUN\r\n");
        delayms(1000);
    }
	return 0;
}
 
void EPIT1_IRQHander_Callback(void)
{
    static int led_state = 0;
    led_state = !led_state;
    led_switch(led_state);
}