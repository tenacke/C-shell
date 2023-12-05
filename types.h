#ifndef TYPES_H
#define TYPES_H

#ifndef MAXIMUM_HISTORY
#define MAXIMUM_HISTORY 256
#endif
#ifndef MAXIMUM_ALIAS
#define MAXIMUM_ALIAS 256
#endif


typedef enum {
    NO_CMD,
    INV_ARG,
    NO_ARG,
    RUN_ERR
} ERR_CODE;

typedef enum {
    NOT_FOUND,
    RUNNING,
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
    REDIR_REVERSE
} OP_TYPE;

typedef struct {
    char* word;
    TOKEN_TYPE type;
} token_t;
typedef struct {
    token_t* tokens;
    int size;
} token_list_t;
typedef struct {
    char* cmd;
    token_list_t* tokens;
    int argc;
    OP_TYPE type;
    int background;
} command_t;

typedef struct {
    char** array;
    int pointer;
    int size;
} Stack;

#endif