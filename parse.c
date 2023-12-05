#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "parse.h"
#include "types.h"
#include "strlib.h"
#include "alias.h"

char* replace_alias(char** dest, char* line) {
    // get string before first whitespace replace with alias and append the rest
    char* cmd = strdup(line);
    char* alias = strtok(cmd, WHITESPACE);
    if (alias == NULL) {
        free(cmd);
        free(*dest);
        *dest = NULL;
        return NULL;
    }
    char* rest = strtok(NULL, "");
    char* alias_cmd = get_alias(alias);
    if (alias_cmd == NULL) {
        free(cmd);
        *dest = line;
        return line;
    } else {
        int length = strlen(alias_cmd) + 2;
        if (rest != NULL) length += strlen(rest);
        char* new_cmd = malloc(sizeof(char) * length);
        strcpy(new_cmd, alias_cmd); strcat(new_cmd, " "); 
        if (rest != NULL) strcat(new_cmd, rest);
        else strcat(new_cmd, "\n");
        free(cmd); free(line); 
        *dest = new_cmd;
        return new_cmd;
    }
}

token_list_t* tokenize(char *line) {
    char *current_char = line;
    int quote = 0;
    int i = 0;
    TOKEN_TYPE type = NO_TOKEN;
    token_list_t* tokens = (token_list_t*) malloc(sizeof(token_list_t));
    tokens->tokens = (token_t*) malloc(sizeof(token_t) * MAXIMUM_ARG);
    tokens->size = 0;
    while (*(current_char+i)) {
        if (*(current_char+i) == '"' || *(current_char+i) == '\'') {
            if (type == NO_TOKEN) {
                type = WORD;
            }
            quote = !quote;
            i++;
        } else if (strchr(WHITESPACE, *(current_char+i))) {
            if (type == NO_TOKEN || quote) {
                i++;
                continue;
            }
            tokens->tokens[tokens->size].word = malloc(sizeof(char) * (i+1));
            tokens->tokens[tokens->size].type = type;
            strncpy(tokens->tokens[tokens->size].word, current_char, i);
            strtrim(tokens->tokens[tokens->size].word, tokens->tokens[tokens->size].word, WHITESPACE);
            strtrim(tokens->tokens[tokens->size].word, tokens->tokens[tokens->size].word, "\"");
            strtrim(tokens->tokens[tokens->size].word, tokens->tokens[tokens->size].word, "\'");
            if (tokens->size >= MAXIMUM_ARG) {
                return NULL;
            }
            current_char += i;
            tokens->size++;
            type = NO_TOKEN;
            i = 0;
        } else { 
            if (type == NO_TOKEN) {
                if (strchr(OPERATOR, *(current_char+i))) {
                    type = OP;
                } else {
                    type = WORD;
                }
            }
            i++;
        }
    }
    if (type != NO_TOKEN) {
        if (quote) {
            // TODO coz burayi
            return NULL;
        }else {
            tokens->tokens[tokens->size].word = malloc(sizeof(char) * (i+1));
            tokens->tokens[tokens->size].type = type;
            strncpy(tokens->tokens[tokens->size].word, current_char, i);
            strtrim(tokens->tokens[tokens->size].word, tokens->tokens[tokens->size].word, WHITESPACE);
            strtrim(tokens->tokens[tokens->size].word, tokens->tokens[tokens->size].word, "\"");
            tokens->size++;
        }
    }
    tokens->tokens = (token_t*) realloc(tokens->tokens, sizeof(token_t) * tokens->size);
    return tokens;
}

command_t* parse_command(token_list_t* tokens){
    command_t* cmd = (command_t*) malloc(sizeof(command_t));
    cmd->cmd = tokens->tokens[0].word;
    cmd->tokens = tokens;
    cmd->argc = 0;
    cmd->type = NO_OP;
    cmd->background = 0;
    for (int i = 0; i < tokens->size; i++) {
        if (tokens->tokens[i].type == OP) {
            char bit = * strchr(OPERATOR, tokens->tokens[i].word[0]);
            switch (bit) {
            case PIPE_CHAR:
                cmd->type = PIPE;
                break;
            case REDIR_IN_CHAR:
                cmd->type = REDIR_IN;
                break;
            case REDIR_OUT_CHAR:
                if (strcmp(tokens->tokens[i].word, REDIR_APPEND_STR) == 0)
                    cmd->type = REDIR_APPEND;
                else if (strcmp(tokens->tokens[i].word, REDIR_REVERSE_STR) == 0)
                    cmd->type = REDIR_REVERSE;
                else
                    cmd->type = REDIR_OUT;
                break;
            case BACKGROUND_CHAR:
                cmd->background = 1;
            }
            break;
        }
        cmd->argc++;
    }
    return cmd;
}

void free_command(command_t* cmd) {
    for (int i = 0; i < cmd->tokens->size; i++) {
        free(cmd->tokens->tokens[i].word);
    }
    free(cmd->tokens->tokens);
    free(cmd->tokens);
    free(cmd);
}
