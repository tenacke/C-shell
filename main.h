#define MAXIMUM_ARG 128
#define MAXIMUM_HOSTNAME 255
#define USER_ENV "USER"
#ifdef _WIN32
#define PATH_ENV "PATH"
#define HOME_ENV "HOMEPATH"
#define PATH_DELIM ";"
#define PATH_SEPARATOR "\\"
#else
#define PATH_ENV "PATH"
#define HOME_ENV "HOME"
#define PATH_DELIM ":"
#define PATH_SEPARATOR "/"
#endif

void start_interactive();
void start_batch(int argc, char *argv[]);
void execute_line(char *line);