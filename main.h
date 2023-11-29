#ifndef MAIN_H

#ifndef MAXIMUM_ARG
#define MAXIMUM_ARG 128
#endif
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

struct PATH {
    char** paths;
    size_t size;
};

#endif