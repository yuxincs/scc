#ifndef SCC_SYNTAX_HEADER
#define SCC_SYNTAX_HEADER

#include <stdio.h>
#include <stdbool.h>
#include "list.h"

struct _Syntax;
typedef struct _Syntax Syntax;

/* Typedefs for syntax types  */
typedef enum 
{
    IMMEDIATE,
    VARIABLE,
    VARIABLE_TYPE,
    VARIABLE_DECLARATION,
    ARRAY_VARIABLE,
    ARRAY_DECLARATION,
    STRUCT_VARIABLE,
    STRUCT_DECLARATION,
    UNARY_EXPRESSION,
    BINARY_EXPRESSION,
    BLOCK,
    IF_STATEMENT,
    RETURN_STATEMENT,
    FUNCTION_DECLARATION,
    FUNCTION_CALL,
    ASSIGNMENT,
    WHILE_STATEMENT,
    TOP_LEVEL
} SyntaxType;

typedef enum
{
    VOID,
    INT,
    FLOAT,
    STRUCT
} Type;

typedef enum 
{ 
    BITWISE_NEGATION,
    LOGICAL_NEGATION
} UnaryExpressionType;

typedef enum 
{
    ADDITION,
    SUBTRACTION,
    MULTIPLICATION,
    DIVISION,
    EQUAL,
    NOT_EQUAL,
    GREATER,
    LESS,
    GREATER_OR_EQUAL,
    LESS_OR_EQUAL,
} BinaryExpressionType;

typedef struct Immediate 
{
    Type type;
    union
    {
        int int_value;
        float float_value;
    };
} Immediate;

typedef struct VariableType 
{
    Type type;
    char *name;
} VariableType;

typedef struct VariableDeclaration
{
    Syntax *type;
    char *name;
} VariableDeclaration;

typedef struct Variable
{
    char *name;
} Variable;

typedef struct ArrayDeclaration
{
    Syntax *type;
    char *name;
    int length;
} ArrayDeclaration;

typedef struct ArrayVaraible
{
    char *name;
    int index;
} ArrayVariable;

typedef struct StructDeclaration
{
    char *name;
    Syntax *block;
} StructDeclaration;

typedef struct StructVariable
{
    char *name;
    char *member;
} StructVariable;

typedef struct UnaryExpression 
{
    UnaryExpressionType type;
    Syntax *expression;
} UnaryExpression;

typedef struct BinaryExpression 
{
    BinaryExpressionType type;
    Syntax *left;
    Syntax *right;
} BinaryExpression;

typedef struct FunctionDeclaration
{
    Syntax *type;
    char *name;
    Syntax *arguments;
    Syntax *block;
} FunctionDeclaration;

typedef struct FunctionCall 
{
    char *name;
    Syntax *arguments;
} FunctionCall;

typedef struct Assignment 
{
    Syntax *dest;
    Syntax *expression;
} Assignment;

typedef struct IfStatement 
{
    Syntax *condition;
    Syntax *then_body;
    Syntax *else_body;
} IfStatement;

typedef struct WhileStatement 
{
    Syntax *condition;
    Syntax *body;
} WhileStatement;

typedef struct ReturnStatement 
{ 
    Syntax *expression; 
} ReturnStatement;

typedef struct Block 
{ 
    bool can_be_merged;
    List *statements; 
} Block;

typedef struct TopLevel 
{ 
    List *statements; 
} TopLevel;

struct _Syntax
{
    SyntaxType type;
    int lineno;
    union 
    {
        Immediate *immediate;
        Variable *variable;
        VariableDeclaration *variable_declaration;
        VariableType *variable_type;
        ArrayVariable *array_variable;
        ArrayDeclaration *array_declaration;
        StructVariable *struct_variable;
        StructDeclaration *struct_declaration;
        UnaryExpression *unary_expression;
        BinaryExpression *binary_expression;
        Assignment *assignment;
        ReturnStatement *return_statement;
        FunctionCall *function_call;
        IfStatement *if_statement;
        WhileStatement *while_statement;
        Block *block;
        FunctionDeclaration *function_declaration;
        TopLevel *top_level;
    };
};

Syntax * syntax_new(SyntaxType type);
void syntax_delete(Syntax *syntax);
void print_syntax(FILE *fp, Syntax * syntax);

Syntax * syntax_analysis(FILE * source_file);

#endif