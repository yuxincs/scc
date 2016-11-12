#include "symboltable.h"

Symbol * get_symbol(SymbolTable * symbol_table, char * name)
{
    for(int i = list_length(symbol_table->list) - 1; i >= 0; --i)
    {
        Symbol * symbol = list_get(symbol_table->list, i);
        if(strcmp(symbol->name, name) == 0)
            return symbol;
    }
    return NULL;
}

void insert_symbol(SymbolTable * symbol_table, Symbol * symbol)
{
    list_append(symbol_table->list, symbol);
}

SymbolTable * new_symbol_table()
{
    SymbolTable * table = (SymbolTable *)malloc(sizeof(SymbolTable));
    table->list = list_new();
    return table;
}

void remove_level(SymbolTable * table, int level)
{
    while(((Symbol *)list_get(table->list, list_length(table->list) - 1))->level == level)
        list_pop(table->list);
    return;
}