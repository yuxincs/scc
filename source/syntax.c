#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
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
            syntax->immediate->type = INT;
            syntax->immediate->int_value = 0;
            break;
        }
        case VARIABLE_TYPE:
        {
            syntax->variable_type = (VariableType *)malloc(sizeof(VariableType));
            syntax->variable_type->name = string_new(33);
            syntax->variable_type->type = INT;
            break;
        }
        case VARIABLE_DECLARATION:
        {
            syntax->variable_declaration = (VariableDeclaration *)malloc(sizeof(VariableDeclaration));
            syntax->variable_declaration->type = NULL;
            syntax->variable_declaration->name = string_new(33);
            break;
        }
        case VARIABLE:
        {
            syntax->variable = (Variable *)malloc(sizeof(Variable));
            syntax->variable->name = string_new(33);
            break;
        }
        case ARRAY_DECLARATION:
        {
            syntax->array_declaration = (ArrayDeclaration *)malloc(sizeof(ArrayDeclaration));
            syntax->array_declaration->type = NULL;
            syntax->array_declaration->name = string_new(33);
            syntax->array_declaration->length = 0;
            break;
        }
        case ARRAY:
        {
            syntax->array = (Array *)malloc(sizeof(Array));
            syntax->array->name = string_new(33);
            syntax->array->index = 0;
            break;
        }
        case STRUCT_DECLARATION:
        {
            syntax->struct_declaration = (StructDeclaration *)malloc(sizeof(StructDeclaration));
            syntax->struct_declaration->name = string_new(33);
            syntax->struct_declaration->block = NULL;
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
            syntax->if_statement->body = NULL;
            break;
        }
        case RETURN_STATEMENT:
        {
            syntax->return_statement = (ReturnStatement *)malloc(sizeof(ReturnStatement));
            syntax->return_statement->expression = NULL;
            break;
        }
        case FUNCTION_DECLARATION:
        {
            syntax->function_declaration = (FunctionDeclaration *)malloc(sizeof(FunctionDeclaration));
            syntax->function_declaration->type = NULL;
            syntax->function_declaration->name = string_new(33);
            syntax->function_declaration->arguments = NULL;
            syntax->function_declaration->block = NULL;
            break;
        }
        case FUNCTION_CALL:
        {
            syntax->function_call = (FunctionCall *)malloc(sizeof(FunctionCall));
            syntax->function_call->name = string_new(33);
            syntax->function_call->arguments = NULL;
            break;
        }
        case ASSIGNMENT:
        {
            syntax->assignment = (Assignment *)malloc(sizeof(Assignment));
            syntax->assignment->name = string_new(33);
            syntax->assignment->expression = NULL;
            break;
        }
        case WHILE_STATEMENT:
        {
            syntax->while_statement = (WhileStatement *)malloc(sizeof(WhileStatement));
            syntax->while_statement->condition = NULL;
            syntax->while_statement->body = NULL;
            break;
        }
        case TOP_LEVEL:
        {
            syntax->top_level = (TopLevel *)malloc(sizeof(TopLevel));
            syntax->top_level->statements = NULL;
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
            
        }
        case VARIABLE_TYPE:
        {
            
        }
        case VARIABLE_DECLARATION:
        {
            
        }
        case VARIABLE:
        {
            
        }
        case ARRAY_DECLARATION:
        {
            
        }
        case ARRAY:
        {
            
        }
        case STRUCT_DECLARATION:
        {
            
        }
        case UNARY_EXPRESSION:
        {
            
        }
        case BINARY_EXPRESSION:
        {
            
        }
        case BLOCK:
        {
            
        }
        case IF_STATEMENT:
        {
           
        }
        case RETURN_STATEMENT:
        {
            
        }
        case FUNCTION_DECLARATION:
        {
            
        }
        case FUNCTION_CALL:
        {
           
        }
        case ASSIGNMENT:
        {
            
        }
        case WHILE_STATEMENT:
        {
           
        }
        case TOP_LEVEL:
        {

        }
        default: break;
    }
}



void print_syntax_depth(Syntax * syntax, int depth)
{
    if(syntax == NULL)
    {
        PRINT_SPACE(depth)
        printf("(null)\n");
        return;
    }

    switch(syntax->type)
    {
        case IMMEDIATE:
        {
            PRINT_SPACE(depth)
            if(syntax->immediate->type == INT)
                printf("Immediate : %d\n", syntax->immediate->int_value);
            else if(syntax->immediate->type == FLOAT)
                printf("Immediate : %f\n", syntax->immediate->float_value);
            else
                printf("Immediate Type Error!\n");
            break;
        }
        case VARIABLE_TYPE:
        {
            PRINT_SPACE(depth)
            printf("VariableType : %d %s\n", (int)syntax->variable_type->type, syntax->variable_type->name);
            break;
        }
        case VARIABLE_DECLARATION:
        {
            PRINT_SPACE(depth)
            printf("VariableDeclaration : %s\n", syntax->variable_declaration->name);
            print_syntax_depth(syntax->variable_declaration->type, depth + 2);
            break;
        }
        case VARIABLE:
        {
            PRINT_SPACE(depth)
            printf("Variable : %s\n", syntax->variable->name);
            break;
        }
        case ARRAY_DECLARATION:
        {
            
            PRINT_SPACE(depth)
            printf("ArrayDeclaration : %s Length: %d\n", syntax->array_declaration->name, syntax->array_declaration->length);
            print_syntax_depth(syntax->array_declaration->type, depth + 2);
            break;
        }
        case ARRAY:
        {
            PRINT_SPACE(depth)
            printf("Array : %s index: %d\n", syntax->array->name, syntax->array->index);
            break;
        }
        case STRUCT_DECLARATION:
        {
            PRINT_SPACE(depth)
            printf("StructDeclaration : %s\n", syntax->struct_declaration->name);
            print_syntax_depth(syntax->struct_declaration->block, depth + 2);
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
            print_syntax_depth(syntax->if_statement->body, depth + 2);
            break;
        }
        case RETURN_STATEMENT:
        {
            PRINT_SPACE(depth)
            printf("ReturnStatement\n");
            print_syntax_depth(syntax->return_statement->expression, depth + 2);
            break;
        }
        case FUNCTION_DECLARATION:
        {
            PRINT_SPACE(depth)
            printf("FunctionDeclaration : %s\n", syntax->function_declaration->name);
            PRINT_SPACE(depth)
            printf("ReturnType: \n");
            print_syntax_depth(syntax->function_declaration->type, depth + 2);
            PRINT_SPACE(depth)
            printf("Arguments: \n");
            print_syntax_depth(syntax->function_declaration->arguments, depth + 2);
            PRINT_SPACE(depth)
            printf("Body: \n");
            print_syntax_depth(syntax->function_declaration->block, depth + 2);
            break;
        }
        case FUNCTION_CALL:
        {
            PRINT_SPACE(depth)
            printf("FunctionCall : %s\n", syntax->function_call->name);
            PRINT_SPACE(depth)
            printf("Arguments: \n");
            print_syntax_depth(syntax->function_call->arguments, depth + 2);
            break;
        }
        case ASSIGNMENT:
        {
            PRINT_SPACE(depth)
            printf("Assignment : %s\n", syntax->assignment->name);
            print_syntax_depth(syntax->assignment->expression, depth + 2);
            break;
        }
        case WHILE_STATEMENT:
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
        case BLOCK:
        {
            for(int i = 0; i < list_length(syntax->block->statements); ++i)
                print_syntax_depth((Syntax *)list_get(syntax->block->statements, i), depth);
            break;
        }
        case TOP_LEVEL:
        {
            PRINT_SPACE(depth)
            printf("TopLevel\n");
            for(int i = 0; i < list_length(syntax->top_level->statements); ++i)
                print_syntax_depth((Syntax *)list_get(syntax->top_level->statements, i), depth + 2);
            
            break;
        }
        default: printf("Error!Undefined type!\n"); break;
    }
}

