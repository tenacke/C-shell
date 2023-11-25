#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <stdarg.h>
#include "main.h"
#include "print.h"

void myprintf(char *str, command_t cmd, ...){
    // TODO print to file if specified in command
    va_list args;
    va_start(args, cmd);
    vfprintf(stdout, str, args);
    va_end(args);
}

void print_prompt(){
    char *user = getenv(USER_ENV);
    char host[MAXIMUM_HOSTNAME + 1];
    gethostname(host, MAXIMUM_HOSTNAME);
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
