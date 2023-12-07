#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pwd.h>
#include <time.h>
#include <sys/types.h>

#include "builtins.h"
#include "main.h"
#include "alias.h"
#include "shell.h"
#include "process.h"
#include "print.h"
#include "types.h"

extern char* last;

builtin_t builtins[] = {
    {"echo", &echo},
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

SIGNAL echo(command_t* command){
    for (int i = 1; i < command->argc; i++)
    myprintf("%s ", command, command->tokens->tokens[i].word);
    myprintf("\n", command);
    return SUCCESS;
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
    char cmd[100];
    sprintf(cmd, "ps -o args= -p %d", getppid());
    FILE* out = popen(cmd, "r");
    char buf[100];
    fgets(buf, 100, out);
    myprintf("%s", command, buf);

    // home
    myprintf("%s\n", command, getenv(HOME_ENV));

    // date
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    myprintf("%s", command, asctime(tm));
    
    // number of processes
    // TODO ps aux | wc -l
    sprintf(cmd, "ps aux | wc -l");
    out = popen(cmd, "r");
    fgets(buf, 100, out);
    myprintf("%s", command, buf);


    return SUCCESS;
}

SIGNAL exit_(command_t* command){
    exit_shell();
    return SUCCESS;
}
