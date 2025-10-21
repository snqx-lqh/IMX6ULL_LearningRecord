#include "shell.h"
#include "w25q128.h"
#include "xmodem.h"
#include "bsp_spi.h"
/*------------------- UART相关函数 -------------------*/
#include "bsp_uart.h"

static int8_t ShellReadByte(char *c)
{
    UART_ReadBlocking(UART1, (uint8_t*)c, 1);
    return 0;
}

static void ShellWriteByte(const char c)
{
    uart_putc(c);
}

/*------------------- 命令函数声明 -------------------*/
void ShellWriteStr(char *str)
{
	char *p = str;

	while(*p)
		ShellWriteByte(*p++);
}

void cmd_help(int argc, char **argv);
void cmd_led(int argc, char **argv);
void cmd_loadx(int argc, char **argv);
void cmd_flash(int argc, char **argv);

/*------------------- 命令表 -------------------*/
typedef struct
{
    const char *name;
    void (*func)(int argc, char **argv);
    const char *help;
} shell_cmd_t;
 

const shell_cmd_t cmd_table[] = {
    {"help",   cmd_help,   "show all cmd"},
    {"led",    cmd_led,    "control LED: led on/off"},
    {"loadx",  cmd_loadx,  "xmodem receive data to DDR"},
    {"flash",  cmd_flash,  "flash write use"},
};

#define CMD_COUNT (sizeof(cmd_table) / sizeof(cmd_table[0]))

/*------------------- Shell主循环 -------------------*/
#define SHELL_BUF_SIZE 64

void shell_task(void)
{
    char buf[SHELL_BUF_SIZE];
    uint8_t index = 0;
    char c;

    ShellWriteStr(USER_NAME);
    while (1)
    {
        if (ShellReadByte(&c) == 0)
        {
            if (c == '\r' || c == '\n')
            {
                ShellWriteByte('\r');
                ShellWriteByte('\n');
                buf[index] = '\0';  // 结束字符串

                if (index > 0)
                {
                    shell_exec(buf);
                    index = 0;
                }

                ShellWriteStr(USER_NAME);
            }
            else if (c == '\b' && index > 0)  // 退格
            {
                index--;
                ShellWriteByte('\b');
                ShellWriteByte(' ');
                ShellWriteByte('\b');
            }
            else if (index < SHELL_BUF_SIZE - 1)
            {
                buf[index++] = c;
                ShellWriteByte(c); // 回显
            }
        }
    }
}

/*------------------- 命令解析与执行 -------------------*/
void shell_exec(char *cmdline)
{
    char *argv[8] = {0};
    memset(argv, 0, sizeof(argv));
    int argc = 0;
      
    char *token = strtok(cmdline, " ");
 
    while (token && argc < 8)
    {
        argv[argc++] = token;
        token = strtok(NULL, " ");
    } 
    if (argc == 0)
        return; 

    for (int i = 0; i < CMD_COUNT; i++)
    {
        if (strcmp(argv[0], cmd_table[i].name) == 0)
        {
            cmd_table[i].func(argc, argv);
            return;
        }
    }
    uart_printf("unknow cmd: %s\r\n", argv[0]);
}

/*------------------- 命令实现 -------------------*/
void cmd_help(int argc, char **argv)
{
    uart_printf("right cmd:\r\n");
    for (int i = 0; i < CMD_COUNT; i++)
    {
        uart_printf("  %s : %s\r\n", cmd_table[i].name, cmd_table[i].help);
    }
}


void cmd_led(int argc, char **argv)
{

    if (argc < 2)
    {
        uart_printf("用法: led on/off\r\n");
        return;
    }

    if (strcmp(argv[1], "on") == 0)
    {
        uart_printf("LED already on\r\n");
    }
    else if (strcmp(argv[1], "off") == 0)
    {
        uart_printf("LED already off\r\n");
    }
    else
    {
        uart_printf("unknow cmd: %s\r\n", argv[1]);
    }
}

void cmd_loadx(int argc, char **argv)
{
    uint32_t startAddr;
    uint32_t tx_buff[6] = {0x55,0xAA,0x55,0xAA,0x55,0xAA};
    uint32_t rx_buff[6] = {0x55,0xAA,0x55,0xAA,0x55,0xAA};
    spi_read_write_bytes(tx_buff,rx_buff,3);

    //spi_read_write_byte(0x55);
    //spi_read_write_byte(0xAA);
    //spi_read_write_byte(0x55);
}

void cmd_flash(int argc, char **argv)
{
    uart_printf("Receive Start\r\n");
    W25QXX_Erase_Sectors(0,100); // 从地址0开始，擦除100块，也就是100*4KB = 400 KB
    uart_printf("Flash erase finish\r\n");
    XmodemReceiveData(0X400);
    uart_printf("\r\nReceive Finish\r\n");
}