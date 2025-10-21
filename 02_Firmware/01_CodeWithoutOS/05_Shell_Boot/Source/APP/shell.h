#ifndef _SHELL_H
#define _SHELL_H

#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#define USER_NAME "@IMX6ULL:"

void shell_task(void);
void shell_exec(char *cmdline);
void cmd_help(int argc, char **argv);
void cmd_led(int argc, char **argv);

#endif
