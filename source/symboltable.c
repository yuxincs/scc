#include "utils.h"
#include "symboltable.h"
#include "syntax.h"

Symbol * symbol_new()
{
    Symbol * symbol = (Symbol *)malloc(sizeof(Symbol));
    symbol->level = -1;
    symbol->name = string_new(33);
    symbol->declaration = NULL;
    return symbol;
}

void symbol_delete(Symbol * symbol)
{
    free(symbol);
}


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

Symbol * generate_input_function()
{
    Syntax * syntax = syntax_new(FUNCTION_DECLARATION);
    syntax->function_declaration->type = syntax_new(VARIABLE_TYPE);
    syntax->function_declaration->type->variable_type->type = INT;
    strcpy(syntax->function_declaration->name, "__READINT");

    Symbol * symbol = symbol_new();
    symbol->level = 0;
    strcpy(symbol->name, "__READINT");
    symbol->declaration = syntax;
    return symbol;
}

Symbol * generate_output_function()
{
    Syntax * syntax = syntax_new(FUNCTION_DECLARATION);
    syntax->function_declaration->type = syntax_new(VARIABLE_TYPE);
    syntax->function_declaration->type->variable_type->type = VOID;
    strcpy(syntax->function_declaration->name, "__WRITEINT");
    Syntax * arguments = syntax_new(BLOCK);
    arguments->block->statements = list_new();
    Syntax * int_argument = syntax_new(VARIABLE_DECLARATION);
    int_argument->variable_declaration->type = syntax_new(VARIABLE_TYPE);
    int_argument->variable_declaration->type->variable_type->type = INT;
    strcpy(int_argument->variable_declaration->name, "x");

    list_prepend(arguments->block->statements, int_argument);
    syntax->function_declaration->arguments = arguments;

    Symbol * symbol = symbol_new();
    symbol->level = 0;
    strcpy(symbol->name, "__WRITEINT");
    symbol->declaration = syntax;
    return symbol;
}


SymbolTable * symbol_table_new()
{
    SymbolTable * table = (SymbolTable *)malloc(sizeof(SymbolTable));
    table->list = list_new();

    // add default io function symbols
    insert_symbol(table, generate_input_function());
    insert_symbol(table, generate_output_function());

    return table;
}

void symbol_table_delete(SymbolTable * table)
{
    for(int i = 0; i < list_length(table->list); ++i)
    {
        Symbol * symbol = (Symbol *)list_get(table->list, i);
        symbol_delete(symbol);
    }
    list_delete(table->list);
}

void insert_symbol(SymbolTable * symbol_table, Symbol * symbol)
{
    list_append(symbol_table->list, symbol);
}

void remove_symbol_by_level(SymbolTable * table, int level)
{
    while(((Symbol *)list_get(table->list, list_length(table->list) - 1))->level == level)
        list_pop(table->list);
    return;
}
