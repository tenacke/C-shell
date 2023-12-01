#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

#include "process.h"
#include "types.h"
#include "print.h"

struct process_t* head = NULL;
struct process_t* tail = NULL;

pid_t foreground = 0;

void add_process(struct process_t* process);
void remove_process(struct process_t* process);
struct process_t* get_process(pid_t pid);

int create_process(command_t* command, char* path){
    pid_t pid = fork();
    if (pid == 0) {
        setpgid(0, 0);
        char** argv = malloc(sizeof(char*) * (command->argc + 1));
        for (int i = 0; i < command->argc; i++) {
            argv[i] = command->tokens->tokens[i].word;
        }
        execv(path, argv);
        exit(0);
    } else if (pid > 0) {
        if (command->type == BACKGROUND) {
            setpgid(pid, pid);
            tcsetpgrp(STDIN_FILENO, getpid());
            struct process_t* new_process = malloc(sizeof(struct process_t));
            new_process->pid = pid;
            add_process(new_process);
            print_status(RUNNING, pid);
            return 0;
        } else {
            int status;
            foreground = pid;
            waitpid(pid, &status, 0);
            foreground = 0;
            return W_EXITCODE(status, 0);
        }
    }
}

void check_background_processes() {
    struct process_t* current = head;
    while (current != NULL) {
        if (waitpid(current->pid, &current->status, WNOHANG) != 0) {
            if (WIFEXITED(current->status)) {
                if (W_EXITCODE(current->status, 0)) print_status(FAILURE, current->pid);
                else print_status(SUCCESS, current->pid);
                struct process_t* next = current->next;
                remove_process(current);
                free(current);
                current = next;
                continue;
            }
        }
        current = current->next;
    }
}

void add_process(struct process_t* process) {
    if (head == NULL) {
        head = process;
        tail = process;
    } else {
        tail->next = process;
        process->prev = tail;
        tail = process;
    }
}

void remove_process(struct process_t* process) {
    if (process->next) process->next->prev = process->prev;
    else tail = process->prev;
    if (process->prev) process->prev->next = process->next;
    else head = process->next;
}

void kill_all_processes() {
    struct process_t* current = head;
    while (current != NULL) {
        kill(current->pid, SIGHUP);
        current = current->next;
    }
}

void interrupt_handler(int signal) {
    if (foreground) {
        kill(foreground, SIGINT);
    } else {
        
    }
}

struct process_t* get_process(pid_t pid) {
    struct process_t* current = head;
    while (current != NULL) {
        if (current->pid == pid) return current;
        current = current->next;
    }
    return NULL;
}
