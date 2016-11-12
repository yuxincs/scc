#ifndef SCC_SYMBOL_HEADER
#define SCC_SYMBOL_HEADER

#include "syntax.h"

typedef struct Symbol
{
    int level;
    char *name;
    Syntax * declaration;
} Symbol;

Symbol * symbol_new();

void symbol_delete(Symbol * symbol);

#endif