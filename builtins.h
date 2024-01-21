#ifndef BUILTINS_H
#define BUILTINS_H

#include <sys/types.h>
#include "types.h"

typedef SIGNAL (builtin_func)(command_t*);
typedef struct builtin {
    char* name;
    builtin_func* func;
} builtin_t;

builtin_func* get_builtin_func(char *name);

SIGNAL echo(command_t* cmd);
SIGNAL cd(command_t* cmd);
SIGNAL pwd(command_t* cmd);
SIGNAL history(command_t* cmd);
SIGNAL alias(command_t* cmd);
SIGNAL bello(command_t* cmd);
SIGNAL exit_(command_t* cmd);
#endif