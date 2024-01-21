#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "main.h"
#include "shell.h"
#include "parse.h"
#include "print.h"
#include "strlib.h"
#include "builtins.h"   
#include "types.h"
#include "process.h" 

struct PATH PATH;
char* last = NULL;
pid_t parent_pid;

void batch(int argc, char *argv[]);
void interactive();

int main(int argc, char *argv[]) {
    if (argc > 1) {
        batch(argc, argv); // batch mode not implemented
    } else {
        interactive(); // interactive mode
    }
    return 0;
}

void interactive() {
    init_shell();
    parent_pid = getppid(); // get parent pid for bello 
    char *line = NULL;
    size_t len = 0;
    while (1) {
        print_prompt();
        if (getline(&line, &len, stdin) == -1) {
            if (feof(stdin)) {
                printf("\n");
                break;
            } else {
                exit(1);
            }
        }
        if (feof(stdin)) { // for ctrl+d when line is not empty
            printf("\n");
            break;
        }

        if (!replace_alias(&line, line)) continue; // replace alias
        token_list_t* tokens = tokenize(line); // tokenize line

        if (tokens == NULL) {
            print_err(NO_CMD, NULL, NULL);
            free(line);
            line = NULL;
            continue;
        }
        command_t* cmd = parse_command(tokens); // parse tokens
        
        SIGNAL sig = execute_command(cmd); // execute command
        if (sig == SUCCESS){
            if (last != NULL) { // update last command for bello
                free(last);
                last = NULL;
            }
            last = strdup(line);
        }
        if (!cmd->background)
            free_command(cmd); // free command
        else {
            // nanosleep 0.05 seconds if background process terminated immediately, the prompt will be printed properly
            struct timespec ts;
            ts.tv_sec = 0;
            ts.tv_nsec = 50000000L;
            nanosleep(&ts, NULL);
        }
        free(line);
        line = NULL;
    }
    exit_shell();
}

void batch(int argc, char *argv[]) {   // batch mode not implemented for this project
    
}
