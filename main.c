#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include "func.h"

void start_interactive();
void start_batch(int argc, char *argv[]);
void execute_line(char *line);
void init_shell();

Stack *history_stack = NULL;

char** PATH = NULL;

int main(int argc, char *argv[]) {
    if (argc == 1) { start_interactive(); }
    else { start_batch(argc, argv); }
    return 0;
}

void start_interactive(){
    init_shell();
    char line[MAXIMUM_LINE+1];
    print_prompt();
    while (fgets(line, MAXIMUM_LINE, stdin)){
        strtrim(line, line, WHITESPACE);
        if (strchr(WHITESPACE, *line) || *line == '\0') { print_prompt(); continue; }
        execute_line(line);
        print_prompt();
    }
    exit_shell();
}

void start_batch(int argc, char *argv[]) { 
    parse_args(argc, argv);
}

void execute_line(char *line){
    char *line_copy = strdup(line);
    append_history(line_copy);
    command_t command = parse_command(line);
    if (run_built_in(command) == 0) {  }
    else if (run_external(command) == 0) {  } 
    else { print_err(NO_CMD, command, command.command); }
    free_command(command);
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