#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "alias.h"

struct alias_t* aliases;
char* alias_file = ".myshell_aliases";

void add_alias(char *alias, char *command){  // add alias to linked list
    if (aliases == NULL){
        aliases = malloc(sizeof(struct alias_t));
        aliases->alias = strdup(alias);
        aliases->command = strdup(command);
        aliases->next = NULL;
        return;
    }
    struct alias_t* tmp = aliases;
    while(tmp->next != NULL){
        if (strcmp(tmp->alias, alias) == 0){
            free(tmp->command);
            tmp->command = strdup(command);
            return;
        }
        tmp = tmp->next;
    }
    tmp->next = malloc(sizeof(struct alias_t));
    tmp->next->alias = strdup(alias);
    tmp->next->command = strdup(command);
    tmp->next->next = NULL;
}

char* get_alias(char *alias){ // get alias from linked list
    struct alias_t* tmp = aliases;
    while(tmp != NULL){
        if(strcmp(tmp->alias, alias) == 0){
            return tmp->command;
        }
        tmp = tmp->next;
    }
    return NULL;
}

void load_aliases(){    // load aliases from file
    FILE* file = fopen(alias_file, "r");
    if(file == NULL)
        return;
    char* line = NULL;
    size_t len = 0;
    while(getline(&line, &len, file) != -1){
        char* alias = strtok(line, "=");
        char* command = strtok(NULL, "\n");
        add_alias(alias, command);
    }
    fclose(file);
}

void save_aliases(){   // save aliases to file
    FILE* file = fopen(alias_file, "w");
    if(file == NULL)
        return;
    struct alias_t* tmp = aliases;
    while(tmp != NULL){
        fprintf(file, "%s=%s\n", tmp->alias, tmp->command);
        tmp = tmp->next;
    }
    fclose(file);
}

struct alias_t* get_aliases(){
    return aliases;
}