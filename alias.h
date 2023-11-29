#ifndef ALIAS_H
#define ALIAS_H

#define MAXIMUM_ALIAS 256
#define MAXIMUM_ALIAS_EXPANSION 1024
#include "types.h"

void add_alias(char *alias, char *cmd);
char* get_alias(char *alias);
void delete_alias(char *alias);
alias_t* get_aliases();
#endif