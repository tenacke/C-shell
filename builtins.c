#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "builtins.h"
#include "main.h"
#include "print.h"
#include "types.h"


builtin_t builtins[] = {
    {"echo", &echo},
    {"cd", &cd},
    {"pwd", &pwd},
    {"history", &history},
    {"alias", &alias},
    {"bello", &bello}
};

builtin_func* get_builtin_func(char* name){
    for(int i = 0; i < sizeof(builtins)/sizeof(builtin_t); i++){
        if(strcmp(name, builtins[i].name) == 0){
            return builtins[i].func;
        }
    }
    return NULL;
}

SIGNAL echo(command_t* command){
    for (int i = 1; i < command->argc; i++)
    myprintf("%s ", command, command->tokens->tokens[i].word);
    myprintf("\n", command);
    return SUCCESS;
}

SIGNAL pwd(command_t* command){
    char* cwd = getcwd(NULL, 0);
    myprintf("%s\n", command, cwd);
    free(cwd);
    return SUCCESS;
}

SIGNAL cd(command_t* command){
    if (chdir(command->tokens->tokens[1].word) == 0)
    return SUCCESS;
    return FAILURE;
}

SIGNAL alias(command_t* command){
    return SUCCESS;
}

SIGNAL history(command_t* command){
    return SUCCESS;
}

SIGNAL bello(command_t* command){
    // user
    myprintf("%s\n", command, getenv(USER_ENV));

    // host
    char host[MAXIMUM_HOSTNAME + 1];
    gethostname(host, MAXIMUM_HOSTNAME);

    myprintf("%s\n", command, host);
    // TODO last command
    // history

    // tty

    // shell
    // TODO edit current shell
    myprintf("%s\n", command, getenv("SHELL"));

    // home
    myprintf("%s\n", command, getenv(HOME_ENV));

    // date

    // number of processes
    // TODO ps aux | wc -l

    return SUCCESS;
}
