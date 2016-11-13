#include "symboltable.h"
#include "syntax.h"

// private function declarations
Symbol * generate_input_function();
Symbol * generate_output_function();

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

    // add default io function symbols
    insert_symbol(table, generate_input_function());
    insert_symbol(table, generate_output_function());

    return table;
}

void remove_level(SymbolTable * table, int level)
{
    while(((Symbol *)list_get(table->list, list_length(table->list) - 1))->level == level)
        list_pop(table->list);
    return;
}

Symbol * generate_input_function()
{
    Syntax * syntax = syntax_new(FUNCTION_DECLARATION);
    syntax->function_declaration->type = syntax_new(VARIABLE_TYPE);
    syntax->function_declaration->type->variable_type->type = INT;
    strcpy(syntax->function_declaration->name, "readint");

    Symbol * symbol = symbol_new();
    symbol->level = 0;
    strcpy(symbol->name, "readint");
    symbol->declaration = syntax;
}

Symbol * generate_output_function()
{
    Syntax * syntax = syntax_new(FUNCTION_DECLARATION);
    syntax->function_declaration->type = syntax_new(VARIABLE_TYPE);
    syntax->function_declaration->type->variable_type->type = VOID;
    strcpy(syntax->function_declaration->name, "writeint");
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
    strcpy(symbol->name, "writeint");
    symbol->declaration = syntax;
}