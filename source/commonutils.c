#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "commonutils.h"

extern char file_name[1024];
extern char *file_content[1024];

char * string_new(int size)
{
    char * buf = (char *)malloc(sizeof(char) * size);
    buf[0] = '\0';
    return buf;
}

void print_error(char *msg, char *error_part, int lineno)
{ 
    printf("\033[1m%s [%d] \033[1;31merror:\033[0m\033[1m %s \033[0m\n", file_name, lineno, msg);
    printf("%s\n", file_content[lineno - 1]);
    int index = -1;
    char * pos = strstr(file_content[lineno - 1], error_part);
    if(pos != NULL)
        index = pos - file_content[lineno - 1];
    
    printf("\033[1;32m");
    for(int i = 0;i < index; ++i)
        putchar(' ');
    for(int j = 0;j < strlen(error_part);j ++)
        putchar('~');
    putchar('\n');
    printf("\033[0m");
    return;
}

void print_note(char *msg, char *note_part, int lineno)
{
    printf("\033[1m%s [%d] note: %s \033[0m\n", file_name, lineno, msg);
    printf("%s\n", file_content[lineno - 1]);
    int index = -1; 
    char * pos = strstr(file_content[lineno - 1], note_part);
    if(pos != NULL)
        index = pos - file_content[lineno - 1];
    
    printf("\033[1;32m");
    for(int i = 0;i < index;i ++)
        putchar(' ');
    for(int j = 0;j < strlen(note_part);j ++)
        putchar('~');
    putchar('\n');
    printf("\033[0m");
    return;
}