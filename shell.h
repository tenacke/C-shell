#ifndef SHELL_H
#define SHELL_H

#define INIT_FILE ".myshrc"
#define MAXIMUM_LINE 1024
#include "types.h"

void init_shell();
int exit_shell();
SIGNAL execute_command(command_t* command);
SIGNAL execute_external(command_t* command);
char* get_external_path(char* command);

#endif