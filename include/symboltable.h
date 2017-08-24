#pragma once
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

typedef struct _SymbolTable
{
    List *list;
} SymbolTable;

Symbol * symbol_new();
void symbol_delete(Symbol * symbol);


SymbolTable * symbol_table_new();
void symbol_table_delete(SymbolTable * table);
Symbol * get_symbol(SymbolTable * table, char * name);
void insert_symbol(SymbolTable * table, Symbol * symbol);
void remove_symbol_by_level(SymbolTable * tabel, int level);
