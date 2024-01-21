#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "main.h"
#include "strlib.h"
#include "types.h"

char** strsplit(char* line, const char* delim, size_t* size){ // split string by delimiter (helper function)
    char **tokens = (char**) malloc(sizeof(char*) * (strlen(line) + 1));
    char *token = strtok(line, delim);
    int i = 0;
    while (token != NULL){
        tokens[i] = token;
        token = strtok(NULL, delim);
        i++;
    }
    tokens[i] = NULL;
    tokens = (char**) realloc(tokens, sizeof(char*) * (i + 1));
    *size = i;
    return tokens;
}

char* strtrim(char* dest, const char* src, const char* delim){ // trim string by delimiter (helper function)
    const char *start = src;
    const char *end = src + strlen(src) - 1;
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

char* pathjoin(char** dest, const char* s1, const char* s2) { // join two paths (helper function)
    char* result = malloc(strlen(s1) + strlen(s2) + 2);

    if (result) {
        strcpy(result, s1);
        strcat(result, PATH_SEPARATOR);
        strcat(result, s2);
    }

    *dest = result;
    return result;
}