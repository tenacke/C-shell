#ifndef PRINT_H
#define PRINT_H

#define PROMPT_FORMAT "%s@%s %s --- "
#include "types.h"

void myprintf(char *str, command_t* cmd, ...);
void print_prompt();
void print_err(ERR_CODE err, command_t* cmd, char *arg);
void print_err_msg(char *msg, command_t* cmd);
#endif