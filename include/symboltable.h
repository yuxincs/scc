#ifndef SCC_SYMBOLTABLE_HEADER
#define SCC_SYMBOLTABLE_HEADER
#include <stdlib.h>
#include <string.h>
#include "symbol.h"
#include "list.h"

typedef struct _SymbolTable
{
    List *list;
} SymbolTable;

SymbolTable * new_symbol_table();

Symbol * get_symbol(SymbolTable * table, char * name);

void insert_symbol(SymbolTable * table, Symbol * symbol);

void remove_level(SymbolTable * tabel, int level);

#endif