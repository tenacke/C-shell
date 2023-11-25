#ifndef TYPEDEFS_H
#include "types.h"
#endif

void parse_args(int argc, char* argv[]);
command_t parse_command(char *line);
void free_command(command_t cmd);
token_t* tokenize(char *line);