#ifndef SCC_SYMBOLTABLE_HEADER
#define SCC_SYMBOLTABLE_HEADER
#include "symbol.h"

typedef struct _SymbolTable
{

} SymbolTable;

Symbol * get_symbol(SymbolTable * table);

void insert_symbol(SymbolTable * table, Symbol * symbol);

#endif