#include <stdlib.h>
#include "symbol.h"

Symbol * symbol_new(SymbolType type)
{
    Symbol * symbol = (Symbol *)malloc(sizeof(Symbol));
    symbol->type = type;
    switch(symbol->type)
    {
        case VARIABLESYMBOK:
        {
            symbol->variable_symbol = (VariableSymbol *)malloc(sizeof(VariableSymbol));

            break;
        }
        case FUNCTIONSYMBOL:
        {
            symbol->function_symbol = (FunctionSymbol *)malloc(sizeof(FunctionSymbol));
            break;
        }
        case STRUCTSYMBOL:
        {
            symbol->struct_symbol = (StructSymbol *)malloc(sizeof(StructSymbol));
            break;
        }
        default: 
        {
            symbol->variable_symbol = NULL;
            break;
        }
    }
    return symbol;
}

void symbol_delete(Symbol * symbol)
{
    free(symbol);
}