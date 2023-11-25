#include <stdio.h>
#include <stdlib.h>
#include "history.h"

void append_history(char *line){
    if (history_stack == NULL) { return; }
    history_stack->pointer++;
    if (history_stack->pointer == MAXIMUM_HISTORY) { history_stack->pointer = 0; }
    history_stack->array[history_stack->pointer] = line;
    if (history_stack->size < MAXIMUM_HISTORY) { history_stack->size++; }
}

char **get_history(int num){
    if (num > history_stack->size) { num = history_stack->size; }
    char **history = (char**) malloc(sizeof(char*) * num);
    int pointer = history_stack->pointer;
    for (int i = 0; i < num; i++){
        history[i] = history_stack->array[pointer--];
        if (pointer < 0) { pointer = MAXIMUM_HISTORY; }
    }
    for (int i = 0; i < num/2; i++){
        char *temp = history[i];
        history[i] = history[num-i-1];
        history[num-i-1] = temp;
    }
    return history;
}

void save_history(){
    FILE* history_file = fopen(HISTORY_FILE, "w");
    char **history = get_history(history_stack->size);
    for (int i = 0; i < history_stack->size; i++){
        fprintf(history_file, "%s\n", history[i]);
    }
    fclose(history_file);
    free(history);
}