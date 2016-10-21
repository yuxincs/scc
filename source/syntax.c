#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "syntax.h"
#include "list.h"

#define PRINT_SPACE(x) {for(int i = 0;i < (x); ++i) { putchar(' '); }}


// private functions declarations
char * string_new(int size);
Syntax * default_syntax_new(SyntaxType type);
void default_syntax_delete(Syntax * syntax);
void print_syntax_depth(Syntax * syntax, int depth);



Syntax * syntax_new(SyntaxType type)
{
    return default_syntax_new(type);
}

void syntax_delete(Syntax * syntax)
{
    default_syntax_delete(syntax);
    return;
}

void print_syntax(Syntax * syntax)
{
    print_syntax_depth(syntax, 0);
}




char * string_new(int size)
{
    char * buf = (char *)malloc(sizeof(char) * size);
    buf[0] = '\0';
    return buf;
}

Syntax * default_syntax_new(SyntaxType type)
{
    Syntax * syntax = (Syntax *)malloc(sizeof(Syntax));
    syntax->type = type;
    switch(type)
    {
        case IMMEDIATE:
        {
            syntax->immediate = (Immediate *)malloc(sizeof(Immediate));
            syntax->immediate->value = 0;
            break;
        }
        case VARIABLE:
        {
            syntax->variable = (Variable *)malloc(sizeof(Variable));
            syntax->variable->type = NULL;
            syntax->variable->name = (char *)malloc(sizeof(char) * 33);
            break;
        }
        case UNARY_EXPRESSION:
        {
            syntax->unary_expression = (UnaryExpression *)malloc(sizeof(UnaryExpression));
            syntax->unary_expression->type = BITWISE_NEGATION;
            syntax->unary_expression->expression = NULL;
            break;
        }
        case BINARY_EXPRESSION:
        {
            syntax->binary_expression = (BinaryExpression *)malloc(sizeof(BinaryExpression));
            syntax->binary_expression->type = ADDITION;
            syntax->binary_expression->left = NULL;
            syntax->binary_expression->right = NULL;
            break;
        }
        case BLOCK:
        {
            syntax->block = (Block *)malloc(sizeof(Block));
            syntax->block->statements = NULL;
            break;
        }
        case IF_STATEMENT:
        {
            syntax->if_statement = (IfStatement *)malloc(sizeof(IfStatement));
            syntax->if_statement->condition = NULL;
            syntax->if_statement->then = NULL;
            break;
        }
        case RETURN_STATEMENT:
        {
            syntax->return_statement = (ReturnStatement *)malloc(sizeof(ReturnStatement));
            syntax->return_statement->expression = NULL;
            break;
        }
        case FUNCTION:
        {
            syntax->function = (Function *)malloc(sizeof(Function));
            syntax->function->name = string_new(33);
            syntax->function->arguments = NULL;
            syntax->function->root_block = NULL;
            break;
        }
        case FUNCTION_CALL:
        {
            syntax->function_call = (FunctionCall *)malloc(sizeof(FunctionCall));
            syntax->function_call->name = string_new(33);
            syntax->function_call->arguments = NULL;
            break;
        }
        case FUNCTION_ARGUMENTS:
        {
            syntax->function_arguments = (FunctionArguments *)malloc(sizeof(FunctionArguments));
            syntax->function_arguments->arguments = NULL;
            break;
        }
        case ASSIGNMENT:
        {
            syntax->assignment = (Assignment *)malloc(sizeof(Assignment));
            syntax->assignment->name = string_new(33);
            syntax->assignment->expression = NULL;
            break;
        }
        case WHILE_SYNTAX:
        {
            syntax->while_statement = (WhileStatement *)malloc(sizeof(WhileStatement));
            syntax->while_statement->condition = NULL;
            syntax->while_statement->body = NULL;
            break;
        }
        case TOP_LEVEL:
        {
            syntax->top_level = (TopLevel *)malloc(sizeof(TopLevel));
            syntax->top_level->definitions = NULL;
            break;
        }
        case VARIABLE_TYPE:
        {
            syntax->variable_type = (VariableType *)malloc(sizeof(VariableType));
            syntax->variable_type->name = string_new(33);
            syntax->variable_type->type = INT;
            break;
        }
        default: break;
    }
    return syntax;
}

void default_syntax_delete(Syntax * syntax)
{
    switch(syntax->type)
    {
        case IMMEDIATE:
        {
            break;
        }
        case VARIABLE:
        {
            break;
        }
        case UNARY_EXPRESSION:
        {
            break;
        }
        case BINARY_EXPRESSION:
        {
            break;
        }
        case BLOCK:
        {
            break;
        }
        case IF_STATEMENT:
        {
            break;
        }
        case RETURN_STATEMENT:
        {
            break;
        }
        case FUNCTION:
        {
            break;
        }
        case FUNCTION_CALL:
        {
            break;
        }
        case FUNCTION_ARGUMENTS:
        {
            break;
        }
        case ASSIGNMENT:
        {
            break;
        }
        case WHILE_SYNTAX:
        {
            break;
        }
        case TOP_LEVEL:
        {
            break;
        }
        default: break;
    }
}



void print_syntax_depth(Syntax * syntax, int depth)
{
    if(syntax == NULL)
        return;

    switch(syntax->type)
    {
        case IMMEDIATE:
        {
            PRINT_SPACE(depth)
            printf("Immediate : %d\n", syntax->immediate->value);
            break;
        }
        case VARIABLE:
        {
            PRINT_SPACE(depth)
            printf("Variable : %s\n", syntax->variable->name);
            break;
        }
        case UNARY_EXPRESSION:
        {
            PRINT_SPACE(depth)
            printf("UnaryExpression : type %d\n", (int) syntax->unary_expression->type);
            print_syntax_depth(syntax->unary_expression->expression, depth + 2);
            break;
        }
        case BINARY_EXPRESSION:
        {
            PRINT_SPACE(depth)
            printf("BinaryExpression : type %d\n", (int) syntax->binary_expression->type);
            PRINT_SPACE(depth)
            printf("Left:\n");
            print_syntax_depth(syntax->binary_expression->left, depth + 2);
            PRINT_SPACE(depth)
            printf("Right:\n");
            print_syntax_depth(syntax->binary_expression->right, depth + 2);
            break;
        }
        case IF_STATEMENT:
        {
            PRINT_SPACE(depth)
            printf("IfStatement\n");
            PRINT_SPACE(depth)
            printf("Condition:\n");
            print_syntax_depth(syntax->if_statement->condition, depth + 2);
            PRINT_SPACE(depth)
            printf("Then:\n");
            print_syntax_depth(syntax->if_statement->then, depth + 2);
            break;
        }
        case RETURN_STATEMENT:
        {
            PRINT_SPACE(depth)
            printf("ReturnStatement\n");
            print_syntax_depth(syntax->return_statement->expression, depth + 2);
            break;
        }
        case FUNCTION:
        {
            PRINT_SPACE(depth)
            printf("Function : %s\n", syntax->function->name);
            PRINT_SPACE(depth)
            printf("Parameters:\n");
            for(int i = 0; i < list_length(syntax->function->parameters); ++i)
                print_syntax_depth((Syntax *)list_get(syntax->function->parameters, i), depth + 2);
            
            print_syntax_depth(syntax->function->root_block, depth + 2);
            break;
        }
        case FUNCTION_CALL:
        {
            PRINT_SPACE(depth)
            printf("FunctionCall : %s\n", syntax->function_call->name);
            print_syntax_depth(syntax->function_call->arguments, depth + 2);
            break;
        }
        case FUNCTION_ARGUMENTS:
        {
            for(int i = 0; i < list_length(syntax->function_arguments->arguments); ++i)
                print_syntax_depth((Syntax *)list_get(syntax->function_arguments->arguments, i), depth + 2);
            break;
        }
        case ASSIGNMENT:
        {
            PRINT_SPACE(depth)
            printf("Assignment : %s\n", syntax->assignment->name);
            print_syntax_depth(syntax->assignment->expression, depth + 2);
            break;
        }
        case WHILE_SYNTAX:
        {
            PRINT_SPACE(depth)
            printf("WhileStatement\n");
            PRINT_SPACE(depth)
            printf("Condition:\n");
            print_syntax_depth(syntax->while_statement->condition, depth + 2);
            PRINT_SPACE(depth)
            printf("Body:\n");
            print_syntax_depth(syntax->while_statement->body, depth + 2);
            break;
        }
        case TOP_LEVEL:
        {
            PRINT_SPACE(depth)
            printf("TopLevel\n");
            for(int i = 0; i < list_length(syntax->top_level->definitions); ++i)
                print_syntax_depth((Syntax *)list_get(syntax->top_level->definitions, i), depth + 2);
            
            break;
        }
        default: printf("Error!Undefined type!\n"); break;
    }
}