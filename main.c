#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "main.h"
#include "shell.h"
#include "parse.h"
#include "history.h"
#include "strlib.h"
#include "print.h"

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
