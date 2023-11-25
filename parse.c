#include <stdlib.h>
#include <string.h>
#include "main.h"
#include "parse.h"
#include "print.h"
#include "strlib.h"

void parse_args(int argc, char *argv[]){
    // TODO parse args
}

command_t parse_command(char *line){
    // TODO parse aliases
    token_t *tokens = tokenize(line);
    command_t command;
    command.args = (token_t*) malloc(sizeof(token_t) * (MAXIMUM_ARG + 1));
    command.op_args = (token_t*) malloc(sizeof(token_t) * (MAXIMUM_ARG + 1));
    int i, j;
    if (tokens[0].type == WORD){
        command.command = tokens[0];
        for (i = 0; tokens[i].type == WORD; i++){
            command.num_args++;
            command.args[i] = tokens[i];
        }
        command.args[i].type = NO_TOKEN;
        for (j = 0; tokens[i].type != NO_TOKEN; i++, j++){
            command.op_args[j] = tokens[i];
        }
        command.op_args[j].type = NO_TOKEN;
        command.args = (token_t*) realloc(command.args, sizeof(token_t) * (command.num_args + 1));
        command.op_args = (token_t*) realloc(command.op_args, sizeof(token_t) * (j + 1));
    } else {
        print_err(INV_ARG, command, tokens[0].word);
    }


    // char **tokens = strsplit(line, WHITESPACE);
    // command.command = tokens[0].word;
    // command.args = tokens;
    // command.num_args = 0;
    // while (command.args[command.num_args] != NULL){
    //     command.num_args++;
    // }
    // command.shell_op = NULL;
    return command;
}

void free_command(command_t command){
    free(command.args);
}

token_t* tokenize(char *line){
    token_t *tokens = (token_t*) malloc(sizeof(token_t) * (MAXIMUM_ARG + 1));
    char *current;
    TOKEN_TYPE current_token = NO_TOKEN;
    int escaped = 0;
    int op_word = 0;
    int i = 0;
    while (*line != '\0'){
        if (strchr(WHITESPACE, *line) != NULL){
            if (!escaped && current_token != NO_TOKEN){
                *line = '\0';
                tokens[i].word = current;
                tokens[i].type = current_token;
                current_token = NO_TOKEN;
                i++;
            }
        }else if (strchr(STR_ESCAPE, *line) != NULL){
            if (escaped){
                escaped = 0;
            } else {
                escaped = 1;
            }
        } else if (strchr(OPERATORS, *line) != NULL){
            if (current_token != NO_TOKEN){
                current = line;
                current_token = OP;
            }
        } else {
            if (current_token == NO_TOKEN) {
                current = line;
                if (op_word) { current_token = OP_WORD; }
                else { current_token = WORD; }
            }
        }
        line++;
    }
    if (current_token != NO_TOKEN){
        tokens[i].word = current;
        tokens[i].type = current_token;
        i++;
    }
    tokens = (token_t*) realloc(tokens, sizeof(token_t) * (i + 1));
    tokens[i].type = NO_TOKEN;
    return tokens;
}