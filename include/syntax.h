#ifndef SCC_SYNTAX_HEADER
#define SCC_SYNTAX_HEADER

struct _List;
typedef struct _List List;
struct _Syntax;
typedef struct _Syntax Syntax;

/* Typedefs for syntax types  */
typedef enum 
{
    IMMEDIATE,
    VARIABLE,
    UNARY_EXPRESSION,
    BINARY_EXPRESSION,
    BLOCK,
    IF_STATEMENT,
    RETURN_STATEMENT,
    FUNCTION,
    FUNCTION_CALL,
    FUNCTION_ARGUMENTS,
    ASSIGNMENT,
    WHILE_SYNTAX,
    TOP_LEVEL,
    VARIABLE_TYPE
} SyntaxType;

typedef enum
{
    VOID,
    INT,
    FLOAT,
    ARRAY,
    STRUCTURE
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
    GREATER_THAN,
    GREATER_THAN_OR_EQUAL,
    LESS_THAN,
    LESS_THAN_OR_EQUAL,
} BinaryExpressionType;

typedef struct Immediate 
{ 
    int value; 
} Immediate;

typedef struct Variable 
{
    Syntax *type;
    char *name;
} Variable;

typedef struct VariableType 
{
    Type type;
    char *name;
} VariableType;

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

typedef struct FunctionArguments 
{ 
    List *arguments; 
} FunctionArguments;

typedef struct FunctionCall 
{
    char *name;
    Syntax *arguments;
} FunctionCall;

typedef struct Assignment 
{
    char *name;
    Syntax *expression;
} Assignment;

typedef struct IfStatement 
{
    Syntax *condition;
    Syntax *then;
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
    List *statements; 
} Block;

typedef struct Function 
{
    Syntax *type;
    char *name;
    Syntax *arguments;
    Syntax *root_block;
} Function;

typedef struct Parameter 
{
    // TODO: once we have other types, we will need to store type here.
    char *name;
} Parameter;

typedef struct TopLevel 
{ 
    List *statements; 
} TopLevel;

struct _Syntax
{
    SyntaxType type;
    union 
    {
        Immediate *immediate;
        Variable *variable;
        VariableType *variable_type;
        UnaryExpression *unary_expression;
        BinaryExpression *binary_expression;
        Assignment *assignment;
        ReturnStatement *return_statement;
        FunctionArguments *function_arguments;
        FunctionCall *function_call;
        IfStatement *if_statement;
        WhileStatement *while_statement;
        Block *block;
        Function *function;
        TopLevel *top_level;
    };
};

Syntax * syntax_new(SyntaxType type);
void syntax_delete(Syntax * syntax);
void print_syntax(Syntax * syntax);


#endif