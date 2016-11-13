#ifndef SCC_SYMBOL_HEADER
#define SCC_SYMBOL_HEADER

#include "syntax.h"

typedef struct Symbol
{
    int level;
    char *name;

    // used in semantic analysis
    Syntax * declaration;

    // used in intermediate code generation
    char *var_name;
} Symbol;

Symbol * symbol_new();

void symbol_delete(Symbol * symbol);

#endif