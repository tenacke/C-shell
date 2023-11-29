#include <string.h>

#include "alias.h"

void add_alias(char *alias, char *command){

}

char* get_alias(char *alias){
    if (alias == NULL) return NULL;
    if (strcmp(alias, "emre") == 0)
    return "make";
    return NULL;
}

void delete_alias(char *alias){

}

alias_t* get_aliases(){
    
}