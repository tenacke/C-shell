#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>  
#include <pwd.h>
#include <time.h>
#include <sys/types.h>
#include <poll.h>

#include "builtins.h"
#include "main.h"
#include "alias.h"
#include "shell.h"
#include "process.h"
#include "print.h"
#include "types.h"

extern char* last;

builtin_t builtins[] = {
    {"cd", &cd},
    {"pwd", &pwd},
    {"history", &history},
    {"alias", &alias},
    {"bello", &bello},
    {"exit", &exit_}
};

builtin_func* get_builtin_func(char* name){
    for(int i = 0; i < sizeof(builtins)/sizeof(builtin_t); i++){
        if(strcmp(name, builtins[i].name) == 0){
            return builtins[i].func;
        }
    }
    return NULL;
}

SIGNAL pwd(command_t* command){
    char* cwd = getcwd(NULL, 0);
    myprintf("%s\n", command, cwd);
    free(cwd);
    return SUCCESS;
}

SIGNAL cd(command_t* command){
    if (chdir(command->tokens->tokens[1].word) == 0)
        return SUCCESS;
    return FAILURE;
}

SIGNAL alias(command_t* command){
    if (command->argc != 4) {
        myprintf("alias: usage: alias [name] = [command]\n", NULL);
        return FAILURE;
    }
    char* alias;
    char* cmd;
    alias = strdup(command->tokens->tokens[1].word);
    cmd = strdup(command->tokens->tokens[3].word);
    add_alias(alias, cmd);
    return SUCCESS;
}

SIGNAL history(command_t* command){
    return SUCCESS;
}

SIGNAL bello(command_t* command){
    // user
    uid_t uid = geteuid();
    myprintf("%s\n", command, getpwuid(uid)->pw_name);
    if (command->type != NO_OP && command->type != REDIR_REVERSE)
        command->type = REDIR_APPEND;
    
    // host
    char host[MAXIMUM_HOSTNAME + 1];
    gethostname(host, MAXIMUM_HOSTNAME);
    myprintf("%s\n", command, host);

    // history
    if (last != NULL)
        myprintf("%s", command, last);
    else 
        myprintf("%s\n", command, "No history");

    // tty
    char* tty = ttyname(STDIN_FILENO);
    myprintf("%s\n", command, tty);
    
    // shell ps -o args= -p
    char pids[10];
    sprintf(pids, "%d", getppid());
    char* args[] = {"ps", "-o", "args=", "-p", pids, NULL};
    int fd[2];
    pipe(fd);
    switch (fork()) {
    case 0:
        close(fd[0]);
        dup2(fd[1], STDOUT_FILENO);
        close(fd[1]);
        execvp("ps", args);
        break;
    
    default:
        close(fd[1]);
        wait(NULL);
        char buf[MAXIMUM_LINE];
        read(fd[0], buf, MAXIMUM_LINE);
        close(fd[0]);
        myprintf("%s", command, buf);
    }

    // home
    myprintf("%s\n", command, getenv(HOME_ENV));

    // date
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    myprintf("%s", command, asctime(tm));
    
    // number of processes
    // TODO ps aux | wc -l
    char* args2[] = {"ps", "-t", tty, NULL};
    int p[2];
    pipe(p);
    pid_t pid = fork();
    // switch (pid = fork()) {
    if (pid == 0) {
        close(p[0]);
        dup2(p[1], STDOUT_FILENO);
        close(p[1]);
        execvp("ps", args2);
    } else {
        close(p[1]);
        // close(p[0]);
        wait(NULL);
        char chr;
        int size = 0;
        while (read(p[0], &chr, sizeof(char)) > 0) 
            if (chr == '\n')
                size++;
        close(p[0]);
        myprintf("%d\n", command, size);
    }

    return SUCCESS;
}

int canReadFromPipe(int fd){
    //file descriptor struct to check if POLLIN bit will be set
    //fd is the file descriptor of the pipe
    struct pollfd fds;
    fds.fd = fd;
    int res;
    //poll with no wait time
    res = poll(&fds, 1, 0);

    //if res < 0 then an error occurred with poll
    //POLLERR is set for some other errors
    //POLLNVAL is set if the pipe is closed
    if(res < 0||fds.revents&(POLLERR|POLLNVAL))
    {
        //an error occurred, check errno
    }
    return fds.revents&POLLIN;
}

SIGNAL exit_(command_t* command){
    exit_shell();
    return SUCCESS;
}
