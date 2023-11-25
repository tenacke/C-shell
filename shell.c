#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include "strlib.h"
#include "main.h"
#include "shell.h"
#include "print.h"
#include "history.h"
#include "parse.h"
#include "builtins.h"

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

void init_shell(){
    FILE* init_file = fopen(INIT_FILE, "r");
    FILE* history_file = fopen(HISTORY_FILE, "r");
    PATH = strsplit(getenv(PATH_ENV), PATH_DELIM);
    history_stack = (Stack*) malloc(sizeof(Stack));

    // TODO read aliases from init file
    char line[MAXIMUM_LINE+1];
    while (fgets(line, MAXIMUM_LINE, init_file)){
        strtrim(line, line, WHITESPACE);
        if (strchr(WHITESPACE, *line) || *line == '\0') { continue; }
        execute_line(line);
    }

    // TODO read history from history file
    while (fgets(line, MAXIMUM_HISTORY_LINE, history_file)){
        strtrim(line, line, WHITESPACE);
        if (strchr(WHITESPACE, *line) || *line == '\0') { continue; }
        char* line_copy = strdup(line);
        append_history(line_copy);
    }
    fclose(init_file);
    fclose(history_file);
}

int exit_shell(){
    save_history();
    free(PATH);
    exit(0);
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