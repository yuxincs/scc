#ifndef SCC_COMMONUTILS_HEADER
#define SCC_COMMONUTILS_HEADER

char * string_new(int size);

void print_error(char *msg, char *error_part, int lineno);

void print_note(char *msg, char *info_part, int lineno);

#endif