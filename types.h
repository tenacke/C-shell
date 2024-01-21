#ifndef TYPES_H
#define TYPES_H

#ifndef MAXIMUM_HISTORY
#define MAXIMUM_HISTORY 256
#endif
#ifndef MAXIMUM_ALIAS
#define MAXIMUM_ALIAS 256
#endif


typedef enum { // error code (for correct error message)
    NO_CMD,
    INV_ARG,
    NO_ARG,
    RUN_ERR
} ERR_CODE;

typedef enum { // signal of command (exit status in abstract way)
    NOT_FOUND,
    RUNNING,
    SUCCESS,
    FAILURE
} SIGNAL;

typedef enum { // token type
    NO_TOKEN,
    WORD,
    OP,
    OP_WORD
} TOKEN_TYPE;

typedef enum { // operator type
    NO_OP,
    PIPE,
    REDIR_IN,
    REDIR_OUT,
    REDIR_APPEND,
    REDIR_REVERSE
} OP_TYPE;

typedef struct { // token
    char* word;
    TOKEN_TYPE type;
} token_t;
typedef struct { // list of tokens
    token_t* tokens;
    int size;
} token_list_t;
typedef struct { // command
    char* cmd;
    token_list_t* tokens;
    int argc;
    OP_TYPE type;
    int background;
} command_t;

typedef struct { // generic stack implementation
    char** array;
    int pointer;
    int size;
} Stack;

#endif