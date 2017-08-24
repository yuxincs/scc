#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "utils.h"
#include "syntax.h"
#include "list.h"

#define PRINT_SPACE(fp, x) {for(int i = 0;i < (x); ++i) { fputc(' ', fp); }}

// yylineno exists in scc.y file
extern int yylineno;

// private functions declarations
void print_syntax_depth(FILE *fp, Syntax * syntax, int depth);


void print_syntax(FILE *fp, Syntax * syntax)
{
    print_syntax_depth(fp, syntax, 0);
}

Syntax * syntax_new(SyntaxType type)
{
    Syntax * syntax = (Syntax *)malloc(sizeof(Syntax));
    syntax->type = type;
    // store the current line number into the syntax
    syntax->lineno = yylineno;
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
        case ARRAY_VARIABLE:
        {
            syntax->array_variable = (ArrayVariable *)malloc(sizeof(ArrayVariable));
            syntax->array_variable->name = string_new(33);
            syntax->array_variable->index = 0;
            break;
        }
        case STRUCT_VARIABLE:
        {
            syntax->struct_variable = (StructVariable *)malloc(sizeof(StructVariable));
            syntax->struct_variable->name = string_new(33);
            syntax->struct_variable->member = string_new(33);
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
            syntax->block->can_be_merged = false;
            syntax->block->statements = NULL;
            break;
        }
        case IF_STATEMENT:
        {
            syntax->if_statement = (IfStatement *)malloc(sizeof(IfStatement));
            syntax->if_statement->condition = NULL;
            syntax->if_statement->then_body = NULL;
            syntax->if_statement->else_body = NULL;
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
            syntax->assignment->dest = NULL;
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

void syntax_delete(Syntax * syntax)
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
        case ARRAY_VARIABLE:
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
    free(syntax);
}



void print_syntax_depth(FILE *fp, Syntax * syntax, int depth)
{
    if(syntax == NULL)
    {
        PRINT_SPACE(fp, depth)
        fprintf(fp, "(null)\n");
        return;
    }
    switch(syntax->type)
    {
        case IMMEDIATE:
        {
            PRINT_SPACE(fp, depth)
            if(syntax->immediate->type == INT)
                fprintf(fp, "Immediate -> %d\n", syntax->immediate->int_value);
            else if(syntax->immediate->type == FLOAT)
                fprintf(fp, "Immediate -> %f\n", syntax->immediate->float_value);
            else
                fprintf(fp, "Immediate Type Error!\n");
            break;
        }
        case VARIABLE_TYPE:
        {
            PRINT_SPACE(fp, depth)
            fprintf(fp, "VariableType -> type %d %s\n", (int)syntax->variable_type->type, syntax->variable_type->name);
            break;
        }
        case VARIABLE_DECLARATION:
        {
            PRINT_SPACE(fp, depth)
            fprintf(fp, "VariableDeclaration -> %s\n", syntax->variable_declaration->name);
            print_syntax_depth(fp, syntax->variable_declaration->type, depth + 2);
            break;
        }
        case VARIABLE:
        {
            PRINT_SPACE(fp, depth)
            fprintf(fp, "Variable -> %s\n", syntax->variable->name);
            break;
        }
        case ARRAY_DECLARATION:
        {
            PRINT_SPACE(fp, depth)
            fprintf(fp, "ArrayDeclaration -> %s Length: %d\n", syntax->array_declaration->name, syntax->array_declaration->length);
            print_syntax_depth(fp, syntax->array_declaration->type, depth + 2);
            break;
        }
        case ARRAY_VARIABLE:
        {
            PRINT_SPACE(fp, depth)
            fprintf(fp, "Array -> %s index: %d\n", syntax->array_variable->name, syntax->array_variable->index);
            break;
        }
        case STRUCT_VARIABLE:
        {
            PRINT_SPACE(fp, depth)
            fprintf(fp, "Struct -> %s member: %s\n", syntax->struct_variable->name, syntax->struct_variable->member);
            break;
        }
        case STRUCT_DECLARATION:
        {
            PRINT_SPACE(fp, depth)
            fprintf(fp, "StructDeclaration -> %s\n", syntax->struct_declaration->name);
            print_syntax_depth(fp, syntax->struct_declaration->block, depth + 2);
            break;
        }
        case UNARY_EXPRESSION:
        {
            PRINT_SPACE(fp, depth)
            fprintf(fp, "UnaryExpression -> type %d\n", (int) syntax->unary_expression->type);
            print_syntax_depth(fp, syntax->unary_expression->expression, depth + 2);
            break;
        }
        case BINARY_EXPRESSION:
        {
            PRINT_SPACE(fp, depth)
            fprintf(fp, "BinaryExpression -> type %d\n", (int) syntax->binary_expression->type);
            PRINT_SPACE(fp, depth)
            fprintf(fp, "Left:\n");
            print_syntax_depth(fp, syntax->binary_expression->left, depth + 2);
            PRINT_SPACE(fp, depth)
            fprintf(fp, "Right:\n");
            print_syntax_depth(fp, syntax->binary_expression->right, depth + 2);
            break;
        }
        case IF_STATEMENT:
        {
            PRINT_SPACE(fp, depth)
            fprintf(fp, "IfStatement ->\n");
            PRINT_SPACE(fp, depth)
            fprintf(fp, "Condition:\n");
            print_syntax_depth(fp, syntax->if_statement->condition, depth + 2);
            PRINT_SPACE(fp, depth)
            fprintf(fp, "Then:\n");
            print_syntax_depth(fp, syntax->if_statement->then_body, depth + 2);
            PRINT_SPACE(fp, depth)
            fprintf(fp, "Else:\n");
            print_syntax_depth(fp, syntax->if_statement->else_body, depth + 2);
            break;
        }
        case RETURN_STATEMENT:
        {
            PRINT_SPACE(fp, depth)
            fprintf(fp, "ReturnStatement ->\n");
            print_syntax_depth(fp, syntax->return_statement->expression, depth + 2);
            break;
        }
        case FUNCTION_DECLARATION:
        {
            PRINT_SPACE(fp, depth)
            fprintf(fp, "FunctionDeclaration -> %s\n", syntax->function_declaration->name);
            PRINT_SPACE(fp, depth)
            fprintf(fp, "ReturnType: \n");
            print_syntax_depth(fp, syntax->function_declaration->type, depth + 2);
            PRINT_SPACE(fp, depth)
            fprintf(fp, "Arguments: \n");
            print_syntax_depth(fp, syntax->function_declaration->arguments, depth + 2);
            PRINT_SPACE(fp, depth)
            fprintf(fp, "Body: \n");
            print_syntax_depth(fp, syntax->function_declaration->block, depth + 2);

            break;
        }
        case FUNCTION_CALL:
        {
            PRINT_SPACE(fp, depth)
            fprintf(fp, "FunctionCall -> %s\n", syntax->function_call->name);
            PRINT_SPACE(fp, depth)
            fprintf(fp, "Arguments: \n");
            print_syntax_depth(fp, syntax->function_call->arguments, depth + 2);

            break;
        }
        case ASSIGNMENT:
        {
            PRINT_SPACE(fp, depth)
            fprintf(fp, "Assignment ->\n");
            PRINT_SPACE(fp, depth)
            fprintf(fp, "Destination: \n");
            print_syntax_depth(fp, syntax->assignment->dest, depth + 2);
            PRINT_SPACE(fp, depth)
            fprintf(fp, "Expression: \n");
            print_syntax_depth(fp, syntax->assignment->expression, depth + 2);

            break;
        }
        case WHILE_STATEMENT:
        {
            PRINT_SPACE(fp, depth)
            fprintf(fp, "WhileStatement ->\n");
            PRINT_SPACE(fp, depth)
            fprintf(fp, "Condition:\n");
            print_syntax_depth(fp, syntax->while_statement->condition, depth + 2);
            PRINT_SPACE(fp, depth)
            fprintf(fp, "Body:\n");
            print_syntax_depth(fp, syntax->while_statement->body, depth + 2);
            break;
        }
        case BLOCK:
        {
            PRINT_SPACE(fp, depth)
            fprintf(fp, "Block ->\n");
            for(int i = 0; i < list_length(syntax->block->statements); ++i)
                print_syntax_depth(fp, (Syntax *)list_get(syntax->block->statements, i), depth + 2);
            break;
        }
        case TOP_LEVEL:
        {
            PRINT_SPACE(fp, depth)
            fprintf(fp, "TopLevel ->\n");
            for(int i = 0; i < list_length(syntax->top_level->statements); ++i)
            {
                print_syntax_depth(fp, (Syntax *)list_get(syntax->top_level->statements, i), depth + 2);
                fputc('\n', fp);
            }
            break;
        }
        default: fprintf(fp, "Error!Undefined type!\n"); break;
    }
}
