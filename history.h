#ifndef HISTORY_H
#define HISTORY_H

#ifndef MAXIMUM_HISTORY
#define MAXIMUM_HISTORY 256
#endif
#define HISTORY_FILE ".mysh_history"
#ifdef MAXIMUM_LINE
#define MAXIMUM_HISTORY_LINE MAXIMUM_LINE
#else
#define MAXIMUM_HISTORY_LINE 1024
#endif
#include "types.h"

//extern Stack* history_stack;
Stack* history_stack;
void append_history(char *cmd);
char** get_history(int num); 
void save_history();
#endif