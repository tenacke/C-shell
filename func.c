#include <string.h>
#include <stdarg.h>
#include <stdlib.h> 
#include <unistd.h>
#include <stdio.h> 
#include <limits.h>
#include "func.h"

extern char** PATH;
extern FILE *init_file;
extern FILE *history_file;

void myprintf(char *str, command_t cmd, ...){
    // TODO print to file if specified in command
    
    va_list args;
    va_start(args, cmd);
    vfprintf(stdout, str, args);
    va_end(args);
}

char** split(char *line, char *delim){
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

command_t parse_command(char *line){
    // TODO parse aliases
    command_t command;
    char **tokens = split(line, WHITESPACE);
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
    int i;
    for (i = 1; i < command.num_args; ++i){
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
    return 0;
}

int bello(command_t command){
    myprintf("%s\n", command, getenv(USER_ENV));
    char host[_POSIX_HOST_NAME_MAX + 1];
    gethostname(host, _POSIX_HOST_NAME_MAX);
    myprintf("%s\n", command, host);
    // TODO last command
    command_t cmd = parse_command("tty");
    run_external(cmd);
    free_command(cmd);
    // TODO edit current shell
    myprintf("%s\n", command, getenv("SHELL"));
    cmd = parse_command("date");
    run_external(cmd);
    free_command(cmd);
    // TODO ps aux | wc -l
    
    return 0;
}

int exit_shell(){
    // TODO close files, free memory, kill background children 
    fclose(init_file);
    fclose(history_file);
    free(PATH);
    exit(0);
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

void save_history(command_t cmd){
    // TODO save history to file
}
