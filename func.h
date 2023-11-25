#define MAXIMUM_ARG 128
#define INIT_FILE ".myshrc"
#define HISTORY_FILE ".mysh_history"
#define USER_ENV "USER"
#define PROMPT_FORMAT "%s@%s %s --- "
#define MAXIMUM_LINE 1024
#define MAXIMUM_ALIAS 256
#define MAXIMUM_ALIAS_EXPANSION 1024
#define MAXIMUM_HISTORY 256
#define MAXIMUM_HISTORY_LINE MAXIMUM_LINE
#define WHITESPACE " \t\n"
#ifdef _WIN32
#define PATH_SEPARATOR "\\"
#define PATH_ENV "PATH"
#define HOME_ENV "HOMEPATH"
#define PATH_DELIM ";"
#else
#define PATH_SEPARATOR "/"
#define PATH_ENV "PATH"
#define HOME_ENV "HOME"
#define PATH_DELIM ":"
#endif

typedef struct {
    char *alias;
    char *cmd;
} alias_t;
typedef struct {
    char *array[MAXIMUM_HISTORY];
    int pointer;
    int size;
} Stack;
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
typedef int (*ptr)(command_t);

void add_alias(char *alias, char *cmd);
void append_history(char *cmd);
char** get_history(int num); 
void save_history();

int run_built_in(command_t command);
int run_external(command_t command);

void myprintf(char *str, command_t cmd, ...);
void print_prompt();
void print_err(ERR_CODE err, command_t cmd, char *arg);
void print_err_msg(char *msg, command_t cmd);

char** strsplit(char* str, char* delim);
char* strtrim(char* dest, char* src, char* delim);

void parse_args(int argc, char* argv[]);
command_t parse_command(char *line);
void free_command(command_t cmd);

int echo(command_t cmd);
int cd(command_t cmd);
int pwd(command_t cmd);
int history(command_t cmd);
int alias(command_t cmd);
int bello(command_t cmd);
ptr get_builtin_func(command_t cmd);
int exit_shell();

char *get_external_path(char *cmd);