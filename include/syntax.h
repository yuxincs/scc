#ifndef SCC_SYNTAX_HEADER
#define SCC_SYNTAX_HEADER

typedef struct _Syntax
{
    int type;
    union value_union{
        int int_value;
        char string_value[128];
    }value;
    int lineno;
    struct _Syntax * upper_layer;
    struct _Syntax * next;
    struct _Syntax * lower_layer;
}Syntax;

#endif