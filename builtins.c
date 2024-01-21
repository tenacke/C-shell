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
extern pid_t parent_pid;

builtin_t builtins[] = { // list of builtins
    {"cd", &cd},
    {"pwd", &pwd},
    {"alias", &alias},
    {"bello", &bello},
    {"exit", &exit_}
};

builtin_func* get_builtin_func(char* name){ // get builtin function from name
    for(int i = 0; i < sizeof(builtins)/sizeof(builtin_t); i++){
        if(strcmp(name, builtins[i].name) == 0){
            return builtins[i].func;
        }
    }
    return NULL;
}

SIGNAL pwd(command_t* command){ // print working directory
    char* cwd = getcwd(NULL, 0);
    myprintf("%s\n", command, cwd);
    free(cwd);
    return SUCCESS;
}

SIGNAL cd(command_t* command){ // change directory
    if (chdir(command->tokens->tokens[1].word) == 0)
        return SUCCESS;
    return FAILURE;
}

SIGNAL alias(command_t* command){ // add alias
    char* alias;
    char* cmd = calloc(MAXIMUM_LINE, sizeof(char));
    alias = strdup(command->tokens->tokens[1].word);
    // parser dont remove quotes for alias
    for (int i = 3; i < command->argc; i++){
        strcat(cmd, command->tokens->tokens[i].word);
        strcat(cmd, " ");
    } 
    strcat(cmd, "\0");
    // so we need to remove them
    if (cmd[0] == '\"') {
        cmd[strlen(cmd) - 2] = '\0';
        cmd++;
    }
    add_alias(alias, cmd);
    return SUCCESS;
}

SIGNAL bello(command_t* command){
    // user
    uid_t uid = geteuid(); // get user id
    myprintf("%s\n", command, getpwuid(uid)->pw_name); // get user name from id (credit: whoami.c from GNU coreutils)
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
    
    // shell ps -o args= -p <pid>
    char pids[10];
    sprintf(pids, "%d", parent_pid); // convert pid to string
    char* args[] = {"ps", "-o", "args=", "-p", pids, NULL}; // ps -o args= -p <pid>
    int fd[2];
    pipe(fd);
    switch (fork()) { // fork and execute ps -o args= -p <pid>
    case 0:
        close(fd[0]);
        dup2(fd[1], STDOUT_FILENO);
        close(fd[1]);
        execvp("ps", args);
        break;
    
    default: // read output from pipe and print to handle redirection with custom print function
        close(fd[1]);
        wait(NULL);
        char buf[MAXIMUM_LINE];
        read(fd[0], buf, MAXIMUM_LINE);
        close(fd[0]);
        myprintf("%s", command, buf);
    }

    // home
    myprintf("%s\n", command, getenv(HOME_ENV));

    // date in roman numerals
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    int year = tm->tm_year + 1900;
    int month = tm->tm_mon + 1;
    int day = tm->tm_mday;
    int del[] = {1000,900,500,400,100,90,50,40,10,9,5,4,1}; // Key value in Roman counting
    char * sym[] = { "M", "CM", "D", "CD", "C", "XC", "L", "XL", "X", "IX", "V", "IV", "I" }; //Symbols for key values
    char res[64] = "\0";         //result string
    int i = 0;                   
    while (year){                 
        while (year/del[i]){    
            strcat(res, sym[i]);
            year -= del[i];       
        }
        i++;                     
    }
    strcat(res, "-"); i = 0;
    while (month){                
        while (month/del[i]){    
            strcat(res, sym[i]);
            month -= del[i];      
        }
        i++;                   
    }
    strcat(res, "-"); i = 0;
    while (day){             
        while (day/del[i]){    
            strcat(res, sym[i]);
            day -= del[i];       
        }
        i++;                     
    }
    myprintf("%s\n", command, res);
    
    // processes
    myprintf("%d\n", command, num_processes()+1);

    return SUCCESS;
}

SIGNAL exit_(command_t* command){
    exit_shell();
    return SUCCESS;
}
