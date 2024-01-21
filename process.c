#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <stdio.h>
#include <time.h>

#include "process.h"
#include "parse.h"
#include "types.h"
#include "print.h"

struct process_t* head = NULL;
struct process_t* tail = NULL;

pid_t foreground = 0;
extern char* last;

void add_process(struct process_t* process);
void remove_process(struct process_t* process);
void handle_redirection(int fd, command_t* command);
struct process_t* get_process(pid_t pid);

int create_process(command_t* command, char* path){
    int p[2];
    pipe(p);
    pid_t pid = fork();
    if (pid == 0) {
        if (command->background){ // background process should not interfere STDIN
            close(STDIN_FILENO);
        }
        if (command->type != NO_OP) { // redirection
            switch (command->type) {
            case REDIR_OUT:
                close(p[0]); // close read end
                close(p[1]); // close write end
                int fd = open(command->tokens->tokens[command->argc + 1].word, O_WRONLY | O_CREAT | O_TRUNC | O_NONBLOCK, 0644); 
                dup2(fd, STDOUT_FILENO);
                close(fd);
                break;

            case REDIR_APPEND:
                close(p[0]); // close read end
                close(p[1]); // close write end
                int fd_append = open(command->tokens->tokens[command->argc + 1].word, O_WRONLY | O_CREAT | O_APPEND | O_NONBLOCK, 0644);
                dup2(fd_append, STDOUT_FILENO);
                close(fd_append);
                break;
            
            case REDIR_REVERSE: // write to pipe for reverse redirection
                close(p[0]); // close read end
                dup2(p[1], STDOUT_FILENO);
                close(p[1]); // close duplicate write end
                break;

            default: // NO_OP
                close(p[1]); // close write end
                close(p[0]); // close read end
                break;
            }
        }
        // execute command
        char** argv = malloc(sizeof(char*) * (command->argc + 1));
        for (int i = 0; i < command->argc; i++) {
            argv[i] = command->tokens->tokens[i].word;
        }
        argv[command->argc] = NULL;
        execv(path, argv);
        exit(0);
    } else if (pid > 0) {
        if (command->background) { // background process
            close(p[1]); // close write end
            if (command->type != REDIR_REVERSE)
                close(p[0]); // close read end
            // add process to list
            struct process_t* new_process = malloc(sizeof(struct process_t));
            new_process->pid = pid;
            new_process->command = command;
            new_process->fd = p[0];
            add_process(new_process);
            print_status(RUNNING, pid);
            return 0;
        } else { // foreground process
            close(p[1]); // close write end
            int status;
            foreground = pid;
            waitpid(pid, &status, 0);
            foreground = 0;
            // handle redirection if reverse redirection
            handle_redirection(p[0], command);
            close(p[0]);
            // read the pipe to output file
            return W_EXITCODE(status, 0);
        }
    }
}

void check_background_processes() {
    // this function is called after every SIGCHLD signal
    struct process_t* current = head;
    while (current != NULL) { // check every process in a polling way
        if (waitpid(current->pid, &current->status, WNOHANG) != 0) {
            if (WIFEXITED(current->status) || WIFSIGNALED(current->status)) {
                // reaping child process
                if (W_EXITCODE(current->status, 0)) print_status(FAILURE, current->pid);
                else {
                    // change last command for bello
                    print_status(SUCCESS, current->pid);
                    free(last);
                    last = NULL;
                    last = malloc(sizeof(char) * (strlen(current->command->cmd) + 2));
                    strcpy(last, current->command->cmd);
                    strcat(last, "\n");
                }
                // handle redirection if reverse redirection
                handle_redirection(current->fd, current->command);
                struct process_t* next = current->next;
                remove_process(current);
                free_command(current->command);
                free(current);
                current = next;
                continue;
            }
        }
        current = current->next;
    }
}

void add_process(struct process_t* process) { // linked list add process
    if (head == NULL) {
        head = process;
        tail = process;
    } else {
        tail->next = process;
        process->prev = tail;
        tail = process;
    }
}

void remove_process(struct process_t* process) {  // linked list remove process
    if (process->next) process->next->prev = process->prev;
    else tail = process->prev;
    if (process->prev) process->prev->next = process->next;
    else head = process->next;
}

void kill_all_processes() {  // kill all processes in linked list with SIGHUP when shell exits
    struct process_t* current = head;
    while (current != NULL) {
        kill(current->pid, SIGHUP);
        current = current->next;
    }
}

void interrupt_handler(int signal) { // handle SIGINT and send SIGINT to foreground process
    if (foreground) {
        kill(foreground, SIGINT);
    } else {
        // write(STDIN_FILENO, "\n", 1);
    }
}

struct process_t* get_process(pid_t pid) { // get process from pid in linked list
    struct process_t* current = head;
    while (current != NULL) {
        if (current->pid == pid) return current;
        current = current->next;
    }
    return NULL;
}

void handle_redirection(int fd, command_t* command){
    // reverse redirection
    if (command->type == REDIR_REVERSE){
        char* buf = calloc(BUFSIZ, sizeof(char));
        char* my = buf;
        int size;
        int fd_reverse = open(command->tokens->tokens[command->argc + 1].word, O_WRONLY | O_CREAT | O_APPEND | O_NONBLOCK, 0644);
        // read from pipe and write to buffer
        while (read(fd, my, 1) > 0) {
            size++;
            my++;
        }
        for (int i = size - 1; i > 0; i--) {
            // write from buffer to file in reverse order
            my--;
            write(fd_reverse, my, 1);
        }
        close(fd_reverse);
        free(buf);
    }
    close(fd);
}

int num_processes() { // count number of processes in linked list
    int count = 0;
    struct process_t* current = head;
    while (current != NULL) {
        count++;
        current = current->next;
    }
    return count;
}