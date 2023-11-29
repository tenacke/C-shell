#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

#include "main.h"
#include "shell.h"
#include "builtins.h"
#include "types.h"
#include "strlib.h"
#include "print.h"

extern struct PATH PATH;

void init_shell() {
    char* path = getenv(PATH_ENV);
    char** paths = strsplit(path, PATH_DELIM, &PATH.size);
    PATH.paths = paths;
}

int exit_shell() {
}

SIGNAL execute_command(command_t* command) {
    char* cmd = command->cmd;
    builtin_func* func;
    if (func = get_builtin_func(cmd)) { 
        return (*func)(command); 
    } else {
        return execute_external(command);
    }
}

SIGNAL execute_external(command_t* command) {
    char* cmd = command->cmd;
    char* path = get_external_path(cmd);
    if (path == NULL) {
        myprintf("%s: command not found\n", command, cmd);
        return NOT_FOUND;
    }
    pid_t pid = fork();
    if (pid == 0) {
        char** argv = malloc(sizeof(char*) * (command->argc + 1));
        for (int i = 0; i < command->argc; i++) {
            argv[i] = command->tokens->tokens[i].word;
        }
        execv(path, argv);
        exit(0);
    } else if (pid > 0) {
        int status;
        waitpid(pid, &status, 0);
        if (status) return FAILURE;
            return SUCCESS;
        
    }
}

char* get_external_path(char* command){
    for (int i = 0; i < PATH.size; i++) {
        char* path = PATH.paths[i];
        char* fullpath;
        pathjoin(&fullpath, path, command);
        if (access(fullpath, F_OK) == 0) return fullpath;
        free(fullpath);
    }
    return NULL;
}