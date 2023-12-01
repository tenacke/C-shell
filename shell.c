#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>

#include "main.h"
#include "shell.h"
#include "process.h"
#include "builtins.h"
#include "types.h"
#include "strlib.h"
#include "print.h"

extern struct PATH PATH;

void init_shell() {
    char* path = getenv(PATH_ENV);
    char** paths = strsplit(path, PATH_DELIM, &PATH.size);
    PATH.paths = paths;

    signal(SIGCHLD, check_background_processes);
    signal(SIGINT, interrupt_handler);
    // struct sigaction sa;
    // sa.sa_handler = check_background_processes;
    // sigemptyset(&sa.sa_mask);
    // sa.sa_flags = 0;
    // sigaction(SIGCHLD, &sa, NULL);
}

int exit_shell() {
    free(PATH.paths);
    kill_all_processes();
    exit(0);
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
    if (!create_process(command, path)) return SUCCESS;
    return FAILURE;
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