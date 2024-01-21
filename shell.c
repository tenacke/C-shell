#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>

#include "main.h"
#include "shell.h"
#include "alias.h"
#include "process.h"
#include "builtins.h"
#include "types.h"
#include "strlib.h"
#include "print.h"

extern struct PATH PATH;

void init_shell() {
    // initialize shell
    char* path = getenv(PATH_ENV);
    setenv(PATH_ENV, path, 1); // set PATH environment variable so that it can be accessed by other programs (e.g. child bash)
    char** paths = strsplit(path, PATH_DELIM, &PATH.size);
    PATH.paths = paths;
    load_aliases(); // load aliases from file

    signal(SIGCHLD, check_background_processes); // check background processes when child process is terminated
    signal(SIGTERM, exit_shell); // exit safely when SIGTERM is received
    signal(SIGQUIT, exit_shell); // exit safely when SIGQUIT is received
    signal(SIGINT, interrupt_handler); // interrupt foreground process when SIGINT is received
    
    // :D
    printf("⠀⠀⠀⠀⠀⠀⠀⢀⣀⣀⣄⣀⡀\n⠀⠀⠀⠀⢀⣴⠾⠛⠉⠉⠉⠉⠛⠿⣦⡀\n⠀⠀⠀⢠⡿⠁⠀⢀⣠⣤⣤⣄⡀⠀⠈⢿⡆⠀\n⠀⠀⢀⣿⣁⣀⣠⡿⠋⠀⠀⠙⢿⣄⣀⣈⣿⡀⠀⠀\n⠀⠀⢸⣿⠛⠛⢻⣧⠀⠿⠇⠀⣼⡟⠛⠛⣿⡇⠀⠀\n⠀⠀⢸⣿⠀⠀⠀⠙⢷⣦⣴⡾⠋⠀⠀⠀⣿⡇⠀⠀\n⠀⠀⢸⣿⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣿⡇⠀⠀\n⠀⠀⣸⣿⠀⠀⠀⠛⠷⠶⠶⠾⠛⠀⠀⠀⣿⣇⠀⠀\n⠀⣸⣿⣿⢷⣦⣀⣀⣀⣀⣀⣀⣀⣀⣴⡾⣿⣿⣇⠀\n⢠⣿⢸⣿⠀⣿⡏⠉⠉⠉⠉⠉⠉⢹⣿⠀⣿⡇⣿⡄\n⢸⡏⢸⣿⣀⣿⡇⠀⠀⠀⠀⠀⠀⢸⣿⣀⣿⡇⢹⡇\n⢸⡇⠀⢿⣏⠉⠁⠀⠀⠀⠀⠀⠀⠈⠉⣹⡿⠀⢸⡇\n⢸⣿⣤⣌⠛⠷⣶⣶⣶⣶⣶⣶⣶⣶⠾⠛⣡⣤⣿⡇\n⠘⠿⠿⠇⠀⠀⠀⢿⡾⠇⠸⢷⡿⠀⠀⠀⠸⠿⠿⠃\n⠀⠀⠀⠀⠀⠀⠀⠛⠛⠁⠈⠛⠛⠀⠀⠀⠀⠀⠀⠀\nBelloo! Welcome to myshell\n");
}

int exit_shell() { // exit shell safely
    printf("exit\n");
    save_aliases(); // save aliases to file
    free(PATH.paths); // free PATH
    kill_all_processes(); // kill all processes with SIGHUP
    exit(0);
}

SIGNAL execute_command(command_t* command) {
    // execute command (builtin or external)
    char* cmd = command->cmd;
    builtin_func* func;
    if (func = get_builtin_func(cmd)) { 
        return (*func)(command); 
    } else {
        return execute_external(command);
    }
}

SIGNAL execute_external(command_t* command) {
    // execute external command
    char* cmd = command->cmd;
    char* path = get_external_path(cmd); // get path of command
    if (path == NULL) {
        myprintf("%s: command not found\n", command, cmd);
        return NOT_FOUND;
    }
    // create process
    if (!create_process(command, path) && !command->background) return SUCCESS;
    if (command->background)
        return RUNNING;
    return FAILURE;
}

char* get_external_path(char* command){ // get path of command
    for (int i = 0; i < PATH.size; i++) {
        char* path = PATH.paths[i];
        char* fullpath;
        pathjoin(&fullpath, path, command);
        if (access(fullpath, F_OK) == 0) return fullpath;
        free(fullpath);
    }
    return NULL;
}