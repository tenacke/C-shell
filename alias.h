#define MAXIMUM_ALIAS 256
#define MAXIMUM_ALIAS_EXPANSION 1024
#ifndef TYPEDEFS_H
#include "types.h"
#endif

void add_alias(char *alias, char *cmd);
char* get_alias(char *alias);
void delete_alias(char *alias);
alias_t* get_aliases();
