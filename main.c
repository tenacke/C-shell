#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <sys/types.h>
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
char* last = NULL;

void batch(int argc, char *argv[]);
void interactive();
void disable_veof(void);
char* read_line(void);

int main(int argc, char *argv[]) {
    // disable_veof();
    // char line[5];
    // read(STDIN_FILENO, line, 1);
    // printf("%d\n", line[0]);
    // if (getchar() == '\033' && getchar() == '[' && getchar() == 'A') {
    //     printf("up\n");
    // }
    // printf("%d\n", getchar());
    // printf("%d\n", *line);
    // // disable_veof();
    // // echo_lines();
    // return 0;
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
    FILE* file = fopen("/dev/tty", "r");
    // disable_veof();
    while (1) {
        print_prompt();

        // if ((line = read_line()) == NULL) {
        //     free(line);
        //     printf("\n");
        //     break;
        // }
        if (getline(&line, &len, file) == -1) {
            if (feof(file)) {
                printf("\n");
                break;
            } else {
                exit(1);
            }
        }
        if (feof(file)) {
            printf("\n");
            fclose(file);
            file = fopen("/dev/tty", "r");
            continue;
        }
        // printf("len: %d\n", len);
        // printf("line: %d %d %d %d\n", line[0], line[1], line[2], line[3]);
        if (!replace_alias(&line, line)) continue;
        token_list_t* tokens = tokenize(line);
        if (tokens == NULL) {
            print_err(NO_CMD, NULL, NULL);
            free(line);
            line = NULL;
            continue;
        }
        command_t* cmd = parse_command(tokens);
        /*
        for (size_t i = 0; i < cmd->argc; i++)
        {
            printf("token %d: %s\n", i, cmd->tokens->tokens[i].word);
        }
        */
        SIGNAL sig = execute_command(cmd);
        if (last != NULL) free(last);
        last = strdup(line);
        // printf("signal: %d\n", sig);
        if (!cmd->background)
            free_command(cmd);
        free(line);
        line = NULL;
    }
    exit_shell();
}

void batch(int argc, char *argv[]) {
    
}

void disable_veof(void) {
    struct termios t;

    if (tcgetattr(STDIN_FILENO, &t)) 
        exit(EXIT_FAILURE);
    t.c_lflag &= ~ECHOCTL;
    // t.c_lflag &= ~ECHO;
    // t.c_lflag &= ~ICANON;
    // t.c_cc[VEOF] = 0;    
    // t.c_cc[VMIN]  = 1;
    // t.c_cc[VTIME] = 0;
    if (tcsetattr(STDIN_FILENO, TCSANOW, &t))
        exit(EXIT_FAILURE);
}

char* read_line(){
    char* line = calloc(8, sizeof(char));
    int size = 0;
    char* c;
    while(read(STDIN_FILENO, c, 3)){
        if (size % 8 == 7) {
            line = realloc(line, size + 8*sizeof(char));
        }
        switch (*c) {
            case 4: // eof
                if (size == 0) {
                    return NULL;
                }
                break;

            case 10: // newline
                write(STDOUT_FILENO, c, 1);
                line[size++] = *c;
                return line;

            case 27: // escape
                // handle arrow keys
                if (c+1 && *(c+1) == 91) {
                    if (c+2 && *(c+2) == 'A') {
                        // up
                        // arrow_vertical(0);
                    } else if (c+2 && *(c+2) == 'B') {
                        // down
                        // arrow_vertical(1);
                    } else if (c+2 && *(c+2) == 'C') {
                        // right
                        write(STDOUT_FILENO, c, 1);
                    } else if (c+2 && *(c+2) == 'D') {
                        // left
                    }
                }
                break;

            case 127: // backspace
                write(STDOUT_FILENO, "\b \b", 3);
                line[--size] = '\0';
                break;

            default: // normal character
                write(STDOUT_FILENO, c, 1);
                line[size++] = *c;
                break;
        }
    }
}