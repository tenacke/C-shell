#ifndef ALIAS_H
#define ALIAS_H

#define MAXIMUM_ALIAS 256
#define MAXIMUM_ALIAS_EXPANSION 1024

struct alias_t{
    char* alias;
    char* command;
    struct alias_t* next;
};

void add_alias(char *alias, char *cmd);
char* get_alias(char *alias);
void load_aliases();
void save_aliases();
struct alias_t* get_aliases();
#endif