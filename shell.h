#define INIT_FILE ".myshrc"
#define MAXIMUM_LINE 1024
#ifndef TYPEDEFS_H
#include "types.h"
#endif

extern char **PATH;

void init_shell();
int exit_shell();

ptr get_builtin_func(command_t cmd);
char *get_external_path(char *cmd);

int run_built_in(command_t command);
int run_external(command_t command);
