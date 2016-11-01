#ifndef SCC_SYMBOL_HEADER
#define SCC_SYMBOL_HEADER

typedef enum
{
    VARIABLESYMBOL,
    FUNCTIONSYMBOL,
    STRUCTSYMBOL
} SymbolType;

typedef struct VariableSymbol
{

} VariableSymbol;

typedef struct FunctionSymbol
{

} FunctionSymbol;

typedef struct StructSymbol
{

} StructSymbol;

typedef struct Symbol
{
    SymbolType type;
    union
    {
        VariableSymbol * variable_symbol;
        FunctionSymbol * function_symbol;
        StructSymbol * struct_symbol;
    };
} Symbol;

Symbol * symbol_new(SymbolType type);

void symbol_delete(Symbol * symbol);

#endif