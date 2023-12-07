#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <fcntl.h>
#include <stdio.h>

#include "process.h"
#include "parse.h"
#include "types.h"
#include "print.h"

struct process_t* head = NULL;
struct process_t* tail = NULL;

pid_t foreground = 0;

void add_process(struct process_t* process);
void remove_process(struct process_t* process);
void handle_redirection(int fd, command_t* command);
struct process_t* get_process(pid_t pid);

int create_process(command_t* command, char* path){
    int p[2];
    pipe(p);

    pid_t pid = fork();
    if (pid == 0) {
        if (command->background){
            close(STDIN_FILENO);
        }
        if (command->type != NO_OP) {
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
            
            case REDIR_REVERSE:
                close(p[0]); // close read end
                dup2(p[1], STDOUT_FILENO);
                close(p[1]); // close duplicate write end
                break;

            default:
                close(p[1]); // close write end
                close(p[0]); // close read end
                break;
            }
        }
        // close(p[0]); // close read end
        // dup2(p[1], STDOUT_FILENO);
        // close(p[1]); // close duplicate write end
        // setpgid(0, 0);
        // handle_redirection(command);s
        char** argv = malloc(sizeof(char*) * (command->argc + 1));
        for (int i = 0; i < command->argc; i++) {
            argv[i] = command->tokens->tokens[i].word;
        }
        execv(path, argv);
        exit(0);
    } else if (pid > 0) {
        if (command->background) {
            // setpgid(pid, pid);
            // tcsetpgrp(STDIN_FILENO, getpid());
            close(p[1]); // close write end
            if (command->type != REDIR_REVERSE)
                close(p[0]); // close read end
            struct process_t* new_process = malloc(sizeof(struct process_t));
            new_process->pid = pid;
            new_process->command = command;
            new_process->fd = p[0];
            add_process(new_process);
            // print_status(RUNNING, pid);
            // sleep(1);
            return 0;
        } else {
            close(p[1]); // close write end
            int status;
            foreground = pid;
            waitpid(pid, &status, 0);
            foreground = 0;
            handle_redirection(p[0], command);
            close(p[0]);
            // read the pipe to output file
            return W_EXITCODE(status, 0);
        }
    }
}

void check_background_processes() {
    struct process_t* current = head;
    while (current != NULL) {
        if (waitpid(current->pid, &current->status, WNOHANG) != 0) {
            if (WIFEXITED(current->status)) {
                // if (W_EXITCODE(current->status, 0)) print_status(FAILURE, current->pid);
                // else print_status(SUCCESS, current->pid);
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
        // write(STDIN_FILENO, "\n", 1);
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

void handle_redirection(int fd, command_t* command){
    if (command->type == REDIR_REVERSE){
        char* buf = calloc(BUFSIZ, sizeof(char));
        char* my = buf;
        int size;
        int fd_reverse = open(command->tokens->tokens[command->argc + 1].word, O_WRONLY | O_CREAT | O_APPEND | O_NONBLOCK, 0644);
        while (read(fd, my, 1) > 0) {
            size++;
            my++;
        }
        for (int i = size - 1; i > 0; i--) {
            my--;
            write(fd_reverse, my, 1);
        }
        close(fd_reverse);
    }
    close(fd);
}

void handle_redirection1(int fd, command_t* command) {
    char* buf = calloc(BUFSIZ, sizeof(char));            
    char* my = buf;
    int size;
    switch (command->type) {
        case REDIR_OUT:
            int fd_out = open(command->tokens->tokens[command->argc + 1].word, O_WRONLY | O_CREAT | O_TRUNC | O_NONBLOCK, 0644);
            while (read(fd, my, 1) > 0) {
                write(fd_out, my, 1);
                my++;
            }
            close(fd_out);
            break;

        case REDIR_IN:
            break;

        case REDIR_APPEND:
            int fd_append = open(command->tokens->tokens[command->argc + 1].word, O_WRONLY | O_CREAT | O_APPEND | O_NONBLOCK, 0644);
            while (read(fd, my, 1) > 0) {
                write(fd_append, my, 1);
                my++;
            }
            close(fd_append);
            break;  

        case REDIR_REVERSE:
            int fd_reverse = open(command->tokens->tokens[command->argc + 1].word, O_WRONLY | O_CREAT | O_APPEND | O_NONBLOCK, 0644);
            while (read(fd, my, 1) > 0) {
                size++;
                my++;
            }
            for (int i = size - 1; i > 0; i--) {
                my--;
                write(fd_reverse, my, 1);
            }
            close(fd_reverse);
            break;

        case NO_OP:
            while (read(fd, my, 1) > 0) {
                if (*my == 0) break;
                write(STDOUT_FILENO, my, 1);
                my++;
            }
            break;
    }
    free(buf);
}

void handle_redirection2(command_t* command) {
    if (command->type != NO_OP) {
        char* filename = command->tokens->tokens[command->argc + 1].word;
        int fd;
        switch (command->type) {
        case REDIR_OUT:
            fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC | O_NONBLOCK, 0644);
            dup2(fd, STDOUT_FILENO);
            break;
        
        case REDIR_IN:
            fd = open(filename, O_RDONLY | O_NONBLOCK);
            dup2(fd, STDIN_FILENO);
            break;

        case REDIR_APPEND:
            fd = open(filename, O_WRONLY | O_CREAT | O_APPEND | O_NONBLOCK, 0644);
            dup2(fd, STDOUT_FILENO);
            break;

        default:
            break;
        }
        close(fd);
    }
}
