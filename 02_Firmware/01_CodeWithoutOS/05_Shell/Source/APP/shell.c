#include "shell.h"

/*------------------- UART相关函数 -------------------*/
int8_t myShellRead(char *c)
{
    UART_ReadBlocking(UART1, (uint8_t*)c, 1);
    return 0;
}

void myShellWrite(const char c)
{
    uart_putc(c);
}

/*------------------- 命令函数声明 -------------------*/
void cmd_help(int argc, char **argv);
void cmd_led(int argc, char **argv);

/*------------------- 命令表 -------------------*/
typedef struct
{
    const char *name;
    void (*func)(int argc, char **argv);
    const char *help;
} shell_cmd_t;
 
const char help_str[] __attribute__((aligned(4))) = "help";
const char help_str_help[] __attribute__((aligned(4))) = "show all cmd";

const char led_str[]  __attribute__((aligned(4))) = "led";
const char led_str_help[]  __attribute__((aligned(4))) = "control LED: led on/off";

const shell_cmd_t cmd_table[] = {
    {help_str, cmd_help, help_str_help},
    {led_str,  cmd_led,  led_str_help},
};

#define CMD_COUNT (sizeof(cmd_table) / sizeof(cmd_table[0]))

/*------------------- Shell主循环 -------------------*/
#define SHELL_BUF_SIZE 64

void shell_task(void)
{
    char buf[SHELL_BUF_SIZE];
    uint8_t index = 0;
    char c;

    myShellWrite('>');
    while (1)
    {
        if (myShellRead(&c) == 0)
        {
            if (c == '\r' || c == '\n')
            {
                myShellWrite('\r');
                myShellWrite('\n');
                buf[index] = '\0';  // 结束字符串

                if (index > 0)
                {
                    shell_exec(buf);
                    index = 0;
                }

                myShellWrite('>');
            }
            else if (c == '\b' && index > 0)  // 退格
            {
                index--;
                myShellWrite('\b');
                myShellWrite(' ');
                myShellWrite('\b');
            }
            else if (index < SHELL_BUF_SIZE - 1)
            {
                buf[index++] = c;
                myShellWrite(c); // 回显
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
    const char led_on_str[] __attribute__((aligned(4))) = "on";
    const char led_off_help[] __attribute__((aligned(4))) = "off";

    if (argc < 2)
    {
        uart_printf("用法: led on/off\r\n");
        return;
    }

    if (strcmp(argv[1], led_on_str) == 0)
    {
        uart_printf("LED already on\r\n");
    }
    else if (strcmp(argv[1], led_off_help) == 0)
    {
        uart_printf("LED already off\r\n");
    }
    else
    {
        uart_printf("unknow cmd: %s\r\n", argv[1]);
    }
}

