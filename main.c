#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include "func.h"

void start_interactive();
void start_batch(int argc, char *argv[]);
void init_shell();

FILE *init_file;
FILE *history_file;

char** PATH = NULL;

int main(int argc, char *argv[]) {
    if (argc == 1) { start_interactive(); }
    else { start_batch(argc, argv); }
    return 0;
}

void start_interactive(){
    init_shell();
    char line[LINE_MAX+1];
    print_prompt();
    while (fgets(line, LINE_MAX, stdin)){
        command_t command = parse_command(line);
        if (run_built_in(command) == 0) {  }
        else if (run_external(command) == 0) {  } 
        else { print_err(NO_CMD, command, command.command); }
        save_history(command);
        free_command(command);
        print_prompt();
    }
    exit_shell();
}

void start_batch(int argc, char *argv[]) {  }

void init_shell(){
    init_file = fopen(INIT_FILE, "rw");
    history_file = fopen(HISTORY_FILE, "a");
    PATH = split(getenv(PATH_ENV), PATH_DELIM);
    // TODO read aliases from init file
}

void print_prompt(){
    char *user = getenv(USER_ENV);
    char host[_POSIX_HOST_NAME_MAX + 1];
    gethostname(host, _POSIX_HOST_NAME_MAX);
    char *cwd = getcwd(NULL, 0);
    printf(PROMPT_FORMAT, user, host, cwd);
    free(cwd);
}

void print_err(ERR_CODE err, command_t cmd, char *arg){
    if (arg == NULL) { arg = ""; }
    switch (err){
        case NO_CMD:
            myprintf("Command not found: %s\n", cmd, arg);
            break;
        case NO_ARG:
            myprintf("No arguments: %s\n", cmd, arg);
            break;
        case INV_ARG:
            myprintf("Invalid argument: %s\n", cmd, arg);
            break;
        case RUN_ERR:
            myprintf("Error running command: \n", cmd, arg);
            break;
        default:
            myprintf("Unknown error\n", cmd);
            break;
    }
}

void print_err_msg(char *msg, command_t cmd){
    myprintf("%s\n", cmd, msg);
}

int run_built_in(command_t command){
    ptr func = get_builtin_func(command);
    if (func == NULL) { return 1; }
    else { func(command); return 0; }
}

int run_external(command_t command){
    char* path = get_external_path(command.command);
    if (path == NULL) { return 1; }
    else {
        pid_t pid = fork();
        if (pid == 0) { execv(path, command.args); }
        else if (pid < 0) { print_err_msg("Error forking", command); }
        else { wait(NULL); }
        return 0;
    }
    return 0;
}