#include <stdlib.h>
#include "commonutils.h"
#include "symbol.h"


Symbol * symbol_new()
{
    Symbol * symbol = (Symbol *)malloc(sizeof(Symbol));
    symbol->level = -1;
    symbol->name = string_new(33);
    symbol->var_name = string_new(33);
    symbol->declaration = NULL;
    return symbol;
}

void symbol_delete(Symbol * symbol)
{
    free(symbol);
}