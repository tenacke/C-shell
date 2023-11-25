#include <stdlib.h>
#include <unistd.h>
#include "builtins.h"
#include "main.h"
#include "shell.h"
#include "history.h"
#include "print.h"
#include "parse.h"

int echo(command_t command){
    for (int i = 1; i < command.num_args; i++){
        myprintf("%s ", command, command.args[i]);
    }
    myprintf("\n", command);
    return 0;
}

int pwd(command_t command){
    char *cwd = getcwd(NULL, 0);
    myprintf("%s\n", command, cwd);
    free(cwd);
    return 0;
}

int cd(command_t command){
    if (command.num_args <= 1) { print_err(NO_ARG, command, NULL); }
    else if (command.num_args > 2) { print_err(INV_ARG, command, *(command.args+2)); }
    else {
        if (chdir(command.args[1]) == 0) { return 0; }
        else { print_err(RUN_ERR, command, *command.args); }
    }
}

int alias(command_t command){
    myprintf("alias\n", command);
    return 0;
}

int history(command_t command){
    if (command.num_args == 1) { 
        int size = history_stack->size;
        if (size > 10) { size = 10; }
        char **history = get_history(size);
        for (int i = 0; i < size; i++){
            myprintf("%s\n", command, history[i]);
        }
        free(history);
    } else if (command.num_args == 2) {
        int num = atoi(command.args[1]);
        if (num == 0 && *command.args[1] != '0') { print_err(INV_ARG, command, command.args[1]); }
        if (num > history_stack->size) { num = history_stack->size; }
        char **history = get_history(num);
        for (int i = 0; i < num; i++){
            myprintf("%s\n", command, history[i]);
        }
        free(history);
    } else {
        print_err(INV_ARG, command, command.args[2]);
    }
    return 0;
}

int bello(command_t command){
    // user
    myprintf("%s\n", command, getenv(USER_ENV));
    
    // host
    char host[MAXIMUM_HOSTNAME + 1];
    gethostname(host, MAXIMUM_HOSTNAME);
    myprintf("%s\n", command, host);
    
    // history
    char** history = get_history(2);
    if (history_stack->size < 2){ myprintf("%s\n", command, history[0]); }
    else { myprintf("%s\n", command, history[1]); }
    free(history);
    
    // tty
    command_t cmd = parse_command("tty");
    run_external(cmd);
    free_command(cmd);
    
    // shell
    // TODO edit current shell
    myprintf("%s\n", command, getenv("SHELL"));

    // home
    myprintf("%s\n", command, getenv(HOME_ENV));
    
    // date
    cmd = parse_command("date");
    run_external(cmd);
    free_command(cmd);
    
    // number of processes
    // TODO ps aux | wc -l
    
    return 0;
}
