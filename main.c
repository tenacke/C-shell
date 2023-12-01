#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "main.h"
#include "shell.h"
#include "parse.h"
#include "print.h"
#include "strlib.h"
#include "types.h"
#include "process.h" 

struct PATH PATH;

void batch(int argc, char *argv[]);
void interactive();

int main(int argc, char *argv[]) {
    if (argc > 1) {
        batch(argc, argv);
    } else {
        interactive();
    }
    return 0;
}

void interactive() {
    init_shell();
    char *line = NULL;
    size_t len = 0;
    
    while (1) {
        print_prompt();
        if (getline(&line, &len, stdin) == -1) break;
        if (!replace_alias(&line, line)) continue;
        token_list_t* tokens = tokenize(line);
        command_t* cmd = parse_command(tokens);
        /*
        for (size_t i = 0; i < cmd->argc; i++)
        {
            printf("token %d: %s\n", i, cmd->tokens->tokens[i].word);
        }
        */
        SIGNAL sig = execute_command(cmd);
        // printf("signal: %d\n", sig);
        free_command(cmd);
        free(line);
        line = NULL;
    }
    exit_shell();
}

void batch(int argc, char *argv[]) {
    
}