#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "main.h"
#include "strlib.h"
#ifndef TYPEDEFS_H
#include "types.h"
#endif

char** strsplit(char *line, char *delim){
    char **tokens = (char**) malloc(sizeof(char*) * (MAXIMUM_ARG + 1));
    char *token = strtok(line, delim);
    int i = 0;
    while (token != NULL){
        tokens[i] = token;
        token = strtok(NULL, delim);
        i++;
    }
    tokens[i] = NULL;
    tokens = (char**) realloc(tokens, sizeof(char*) * (i + 1));
    return tokens;
}

char* strtrim(char *dest, char *src, char *delim){
    char *start = src;
    char *end = src + strlen(src) - 1;
    while (end > start){
        if (strchr(delim, *start) != NULL) { start++; }
        if (strchr(delim, *end) != NULL) { end--; }
        if (strchr(delim, *start) == NULL && strchr(delim, *end) == NULL) { 
            strncpy(dest, start, end - start + 1);
            dest[end - start + 1] = '\0';
            break;
        }
    }
    return dest;
}