#ifndef PARSE_H
#define PARSE_H

#ifndef MAXIMUM_ARG
#define MAXIMUM_ARG 128
#endif
#define REDIR_IN_CHAR '<'
#define REDIR_OUT_CHAR '>'
#define REDIR_APPEND_STR ">>"
#define REDIR_REVERSE_STR ">>>"
#define PIPE_CHAR '|'
#define BACKGROUND_CHAR '&'
#define OPERATOR "|><&"
#define WHITESPACE " \t\r\n\a"

#include "types.h"


void parse_args(int argc, char* argv[]);
char* replace_alias(char** dest, char* cmd);
token_list_t* tokenize(char *line);
command_t* parse_command(token_list_t* tokens);
void free_command(command_t* cmd);
#endif