#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "main.h"
#include "print.h"
#ifdef __APPLE__
#define REMOVE_LOCAL 1
#else
#define REMOVE_LOCAL 0
#endif

void myprintf(char *str, command_t* cmd, ...){ // print to stdout or file
    FILE* file = stdout; // default to stdout
    if (cmd != NULL && cmd->type != NO_OP) {
        switch (cmd->type) { // open file for redirection
        case REDIR_OUT: 
            file = fopen(cmd->tokens->tokens[cmd->argc + 1].word, "w");
            break;
        
        case REDIR_APPEND:
            file = fopen(cmd->tokens->tokens[cmd->argc + 1].word, "a");
            break;

        case REDIR_REVERSE: // reverse string and write to file
            file = fopen(cmd->tokens->tokens[cmd->argc + 1].word, "a");
            char buff[1024];
            va_list args;
            va_start(args, cmd);
            vsprintf(buff, str, args);
            va_end(args);
            int len = strlen(buff);
            for (int i = len - 1; i >= 0; i--) {
                fputc(buff[i], file);
            }
            break;

        default:
            break;
        }
    }
    va_list args;
    va_start(args, cmd);
    if (cmd == NULL || cmd->type != REDIR_REVERSE) {
        vfprintf(file, str, args); // print to stdout or file (not reverse)
    }
    if (cmd != NULL && cmd->type != NO_OP) { // close file for redirection
        fclose(file);
    }
    va_end(args);
}

void print_prompt(){
    //user@host cwd ---
    char *user = getenv(USER_ENV);
    char host[MAXIMUM_HOSTNAME + 1];
    gethostname(host, MAXIMUM_HOSTNAME);
    if (REMOVE_LOCAL) { // remove ".local" from host name (macOS only) macos prints hostname.local by default
        char* local = host;
        while (*local != '\0') {
            if (*local == '.') {
                *local = '\0';
                break;
            }
            local++;
        }
    }
    char *cwd = getcwd(NULL, 0);
    printf(PROMPT_FORMAT, user, host, cwd);
    free(cwd);
}

void print_err(ERR_CODE err, command_t* cmd, char *arg){ // print error message generic manner
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

void print_err_msg(char *msg, command_t* cmd){ // print error message custom manner
    myprintf("%s\n", cmd, msg);
}

void print_status(SIGNAL signal, pid_t pid){ // print status of process (running, success, failure) for background processes
    switch (signal){
        case NOT_FOUND:
            myprintf("Command not found\n", NULL);
            break;
        case RUNNING:
            myprintf("Running process [%d]\n", NULL, pid);
            break;
        case SUCCESS:
            myprintf("Process exited successfully [%d]\n", NULL, pid);
            break;
        case FAILURE:
            myprintf("Process exited with failure [%d]\n", NULL, pid);
            break;
        default:
            myprintf("Unknown signal\n", NULL);
            break;
    }
}
