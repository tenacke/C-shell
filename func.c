#include <string.h>
#include <stdarg.h>
#include <stdlib.h> 
#include <unistd.h>
#include <stdio.h> 
#include <limits.h>
#include "func.h"

extern char** PATH;
extern Stack *history_stack;

void append_history(char *line){
    if (history_stack == NULL) { return; }
    history_stack->pointer++;
    if (history_stack->pointer == MAXIMUM_HISTORY) { history_stack->pointer = 0; }
    history_stack->array[history_stack->pointer] = line;
    if (history_stack->size < MAXIMUM_HISTORY) { history_stack->size++; }
}

char **get_history(int num){
    if (num > history_stack->size) { num = history_stack->size; }
    char **history = (char**) malloc(sizeof(char*) * num);
    int pointer = history_stack->pointer;
    for (int i = 0; i < num; i++){
        history[i] = history_stack->array[pointer--];
        if (pointer < 0) { pointer = MAXIMUM_HISTORY; }
    }
    for (int i = 0; i < num/2; i++){
        char *temp = history[i];
        history[i] = history[num-i-1];
        history[num-i-1] = temp;
    }
    return history;
}

void save_history(){
    FILE* history_file = fopen(HISTORY_FILE, "w");
    char **history = get_history(history_stack->size);
    for (int i = 0; i < history_stack->size; i++){
        fprintf(history_file, "%s\n", history[i]);
    }
    fclose(history_file);
    free(history);
}

void myprintf(char *str, command_t cmd, ...){
    // TODO print to file if specified in command
    
    va_list args;
    va_start(args, cmd);
    vfprintf(stdout, str, args);
    va_end(args);
}

char** strsplit(char *line, char *delim){
    char **tokens = (char**) malloc(sizeof(char*) * (MAXIMUM_ARG + 1));
    char *token = strtok(line, delim);
    int i = 0;
    while (token != NULL){
        tokens[i] = token;
        token = strtok(NULL, delim);
        i++;
    }
    tokens[i] = NULL;
    tokens = (char**) realloc(tokens, sizeof(char*) * (i + 1));
    return tokens;
}

char* strtrim(char *dest, char *src, char *delim){
    char *start = src;
    char *end = src + strlen(src) - 1;
    while (end > start){
        if (strchr(delim, *start) != NULL) { start++; }
        if (strchr(delim, *end) != NULL) { end--; }
        if (strchr(delim, *start) == NULL && strchr(delim, *end) == NULL) { 
            strncpy(dest, start, end - start + 1);
            dest[end - start + 1] = '\0';
            break;
        }
    }
    return dest;
}

void parse_args(int argc, char *argv[]){
    // TODO parse args
}

command_t parse_command(char *line){
    // TODO parse aliases
    command_t command;
    char **tokens = strsplit(line, WHITESPACE);
    command.command = tokens[0];
    command.args = tokens;
    command.num_args = 0;
    while (command.args[command.num_args] != NULL){
        command.num_args++;
    }
    command.shell_op = NULL;
    return command;
}

void free_command(command_t command){
    free(command.args);
}

ptr get_builtin_func(command_t command){
    if (strcmp(command.command, "cd") == 0) { return &cd; }
    else if (strcmp(command.command, "exit") == 0) { return &exit_shell; }
    else if (strcmp(command.command, "alias") == 0) { return &alias; }
    else if (strcmp(command.command, "echo") == 0) { return &echo; }
    else if (strcmp(command.command, "history") == 0) { return &history; }
    else if (strcmp(command.command, "pwd") == 0) { return &pwd; }
    else if (strcmp(command.command, "bello") == 0) { return &bello; }
    else { return NULL; }
}

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
    printf("alias\n");
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
    char host[_POSIX_HOST_NAME_MAX + 1];
    gethostname(host, _POSIX_HOST_NAME_MAX);
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

char *get_external_path(char *cmd){
    if (strstr(cmd, PATH_SEPARATOR) != NULL) { return cmd; }
    for (int i = 0; PATH[i] != NULL; i++){
        char *path = (char*) malloc(sizeof(char) * (strlen(PATH[i]) + strlen(cmd) + 3));
        strcpy(path, PATH[i]);
        strcat(path, PATH_SEPARATOR);
        strcat(path, cmd);
        strcat(path, "\0");
        if (access(path, X_OK) == 0) { return path; }
        free(path);
    }
    return NULL;
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
