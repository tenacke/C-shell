#ifndef MAXIMUM_HISTORY
#define MAXIMUM_HISTORY 256
#endif
#ifndef MAXIMUM_ALIAS
#define MAXIMUM_ALIAS 256
#endif
#define TYPEDEFS_H

typedef struct {
    char *alias;
    char *cmd;
} alias_t;

typedef struct {
    char *command;
    char **args;
    int num_args;
    char *shell_op;
} command_t;

typedef enum {
    NO_CMD,
    INV_ARG,
    NO_ARG,
    RUN_ERR
} ERR_CODE;

typedef enum {
    NOT_FOUND,
    SUCCESS,
    FAILURE
} SIGNAL;

typedef enum {
    NO_TOKEN,
    WORD,
    BACKGRND,
    PIPE,
    REDIR_IN,
    REDIR_OUT,
    REDIR_OUT_REVERSE,
    REDIR_APPEND
} TOKEN_TYPE;

typedef int (*ptr)(command_t);
typedef struct {
    char *array[MAXIMUM_HISTORY];
    int pointer;
    int size;
} Stack;
typedef struct {
    char *word;
    TOKEN_TYPE type;
} token_t;