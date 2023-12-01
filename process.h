#ifndef PROCESS_H
#define PROCESS_H

#include <sys/types.h>
#include "types.h"

struct process_t {
    pid_t pid;
    int status;
    struct process_t* next;
    struct process_t* prev;    
};

int create_process(command_t* command, char* path);
void interrupt_handler(int signal);
void check_background_processes();
void kill_all_processes();


#endif