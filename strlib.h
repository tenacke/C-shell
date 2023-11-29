#ifndef STRLIB_H
#define STRLIB_H

#define STR_ESCAPE "\"\'"
#ifndef WHITESPACE
#define WHITESPACE " \t\r\n\a"
#endif

char** strsplit(char* str, const char* delim, size_t* size);
char* strtrim(char* dest, const char* src, const char* delim);
char* strreplace(char* dest, const char* src, const char* old, const char* new);
char* pathjoin(char** dest, const char* s1, const char* s2);

#endif