#include <stdlib.h>
#include "main.h"
#include "parse.h"
#include "strlib.h"

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

token_t* tokenize(char *line){
    
    return NULL;
}