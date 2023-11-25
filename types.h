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
    token_t command;
    token_t* args;
    int num_args;
    token_t* op_args;
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
    OP,
    OP_WORD
} TOKEN_TYPE;

typedef enum {
    NO_OP,
    PIPE,
    REDIR_IN,
    REDIR_OUT,
    REDIR_APPEND,
    REDIR_ERR,
    REDIR_ERR_APPEND,
    BACKGROUND
} OP_TYPE;

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