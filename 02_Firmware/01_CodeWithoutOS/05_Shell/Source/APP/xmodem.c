#include "xmodem.h"

#include "bsp_uart.h"  
#include "fsl_uart.h"
#include "w25q128.h"   
#include "fsl_uart.h"
#include "bsp_delay.h"

/* XMODEM 常量 */
#define SOH  0x01
#define STX  0x02
#define EOT  0x04
#define ACK  0x06
#define NAK  0x15
#define CAN  0x18

static void XM_SendByte(uint8_t data)
{
    uart_putc(data);
}

// recv_ch:接收字符 timeout_ms: 超时时间（毫秒），0 表示非阻塞  
static uint8_t XM_RecvByte(uint8_t *recv_ch, uint32_t timeout_ms)
{
    while (1)
    {
        /* 检查是否有接收数据 */
        if (UART_GetStatusFlag(UART1, kUART_RxDataReadyFlag))
        {
            *recv_ch = UART_ReadByte(UART1);
            return 1; // 成功接收
        }

        /* 如果 timeout_ms == 0，表示非阻塞模式 */
        if (timeout_ms == 0)
            return 0;

        /* 否则延时1ms，并递减超时计数 */
        delayms(1);
        if (--timeout_ms == 0)
            return 0; // 超时返回
    }
}

//  清空串口缓存数据  
void UART_FlushHW(void)
{
    uint8_t dummy;
    while (UART_GetStatusFlag(UART1, kUART_RxDataReadyFlag))
    {
        dummy = UART_ReadByte(UART1);
    }
}

int XmodemReceiveData()
{
	uint8_t  pkt[1034];
	uint32_t writeAddr = 0x400;
	uint16_t writeSize = 0;
	int      ret;
	uint8_t  ch;
	char     C_Char = 'C';
	
	while(1)
	{
		for(int i=0;i< 64;i++)
		{
			if(C_Char)
			{
				XM_SendByte('C');
			}
			ret = XM_RecvByte(&ch, 1000);
			if (ret == 1) 
			{
				switch (ch)
				{
					case SOH: 
						writeSize = 128;
						pkt[0] = SOH;
						goto start_receive;
						break;
					case STX:
						writeSize = 1024;
						pkt[0] = STX;
						goto start_receive;
						break;
					case EOT:
						XM_SendByte(ACK);
						goto xmodem_exit;
						break;
					case CAN:
						goto xmodem_exit;
						break;
					default:
						break;
				}
			}
		}
start_receive:
		if(C_Char) C_Char=0;
		for(int i = 1;i < writeSize + 3 + 2;i++)
		{
			ret = XM_RecvByte(pkt+i, 1000);
		}
		W25QXX_Write_NoCheck(&pkt[3],writeAddr,writeSize);
		writeAddr += writeSize;
		XM_SendByte(ACK);
		continue;
	}
xmodem_exit:
	UART_FlushHW();
	return 0;
}