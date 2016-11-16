#ifndef SCC_SYMBOLTABLE_HEADER
#define SCC_SYMBOLTABLE_HEADER
#include <stdlib.h>
#include <string.h>
#include "list.h"
#include "syntax.h"

typedef struct Symbol
{
    int level;
    char *name;

    union
    {
        // used in semantic analysis
        Syntax * declaration;

        // used in intermediate code generation
        char *var_name;

        // used in target code generation
        int address;
    };
    
} Symbol;

Symbol * symbol_new();

void symbol_delete(Symbol * symbol);



typedef struct _SymbolTable
{
    List *list;
} SymbolTable;

SymbolTable * new_symbol_table();

void symbol_table_delete(SymbolTable * table);

Symbol * get_symbol(SymbolTable * table, char * name);

void insert_symbol(SymbolTable * table, Symbol * symbol);

void remove_level(SymbolTable * tabel, int level);

#endif