#ifndef SHELL_H
#define SHELL_H

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
	
#define SHELL_CMD_BUF_LEN	128
#define MAX_ARGUMENT_NUM	16

typedef struct
{
    char * cmd;
    char * desc;
    int (* handler)(int argc, char ** argv);
} shell_cmd_t;

extern void shell_process(void);

void print(const char *fmt, ...);
void print_no_newline(const char *fmt, ...);

#endif
