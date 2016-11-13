#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "commonutils.h"
#include "intercode.h"
#include "symboltable.h"

#define ENTER_SCOPE() { cur_level++; }

#define LEAVE_SCOPE() { remove_level(symbol_table, cur_level); cur_level--; }

extern SymbolTable * symbol_table;

Operator binary_type_to_operator(BinaryExpressionType type)
{
    switch(type)
    {
        case ADDITION: return OP_PLUS;
        case SUBTRACTION: return OP_MINUS;
        case MULTIPLICATION: return OP_MULTIPLY;
        case DIVISION: return OP_DIVIDE;
        case EQUAL: return OP_EQUAL;
        case NOT_EQUAL: return OP_NOT_EQUAL;
        case GREATER: return OP_GREATER;
        case LESS: return OP_LESS;
        case GREATER_OR_EQUAL: return OP_GREATER_OR_EQUAL;
        case LESS_OR_EQUAL: return OP_LESS_OR_EQUAL;
    }
}

Operator unary_type_to_operator(UnaryExpressionType type)
{
    switch(type)
    {
        case BITWISE_NEGATION: return OP_BITWISE_NEGATION;
        case LOGICAL_NEGATION: return OP_LOGICAL_NEGATION;
    }
}

void generate_expression(List * code_list, Syntax * syntax, char * temp)
{
    if(syntax == NULL)
    {
        temp[0] = '\0';
        return;
    }
        
    switch(syntax->type)
    {
        case BINARY_EXPRESSION:
        {
            char left_temp[5];
            char right_temp[5];
            char result_temp[5];
            generate_expression(code_list, syntax->binary_expression->left, left_temp);
            generate_expression(code_list, syntax->binary_expression->right, right_temp);
            Quad * quad = quad_new(binary_type_to_operator(syntax->binary_expression->type));
            strcpy(quad->arg1, left_temp);
            strcpy(quad->arg2, right_temp);
            new_temp(quad->result);
            list_append(code_list, quad);

            strcpy(temp, quad->result);
            break;
        }
        case UNARY_EXPRESSION:
        {
            char result_temp[5];
            generate_expression(code_list, syntax->unary_expression->expression, result_temp);
            Quad * quad = quad_new(unary_type_to_operator(syntax->unary_expression->type));
            strcpy(quad->arg1, result_temp);
            strcpy(quad->result, temp);
            list_append(code_list, quad);

            strcpy(temp, quad->result);
            break;
        }
        case VARIABLE:
        {
            strcpy(temp, syntax->variable->name);
            break;
        }
        case IMMEDIATE:
        {
            char result_temp[5];
            new_temp(result_temp);
            Quad * quad = quad_new(OP_DEFINE);
            sprintf(quad->arg1, "%d", syntax->immediate->int_value);
            strcpy(quad->result, result_temp);
            list_append(code_list, quad);
            break;
        }
        
        default: printf("Error! Undefined type!\n"); break;
    }
}

void generate_intermediate_code(List * code_list, Syntax * syntax)
{
    static int cur_level = 0;
    static Syntax * cur_function = NULL;
    static Syntax * old_function = NULL;

    if(syntax == NULL)
    {
        printf("(null)\n");
        return;
    }
    switch(syntax->type)
    {
        case VARIABLE_DECLARATION:
        {
            Symbol * previous_symbol = get_symbol(symbol_table, syntax->variable_declaration->name);
            
            if(previous_symbol == NULL)
            {
                Symbol *symbol = symbol_new();
                symbol->level = cur_level;
                strcpy(symbol->name, syntax->variable_declaration->name);
                symbol->declaration = syntax;
                insert_symbol(symbol_table, symbol);
            }
            break;
        }
        case VARIABLE:
        {
            Symbol * previous_symbol = get_symbol(symbol_table, syntax->variable->name);
            
            
            break;
        }
        case ARRAY_DECLARATION:
        {
            Symbol * previous_symbol = get_symbol(symbol_table, syntax->array_declaration->name);

            // redefinition check
            if(previous_symbol == NULL)
            {
                Symbol *symbol = symbol_new();
                symbol->level = cur_level;
                strcpy(symbol->name, syntax->array_declaration->name);
                symbol->declaration = syntax;
                insert_symbol(symbol_table, symbol);
            }
            break;
        }
        case ARRAY_VARIABLE:
        {
            Symbol * previous_symbol = get_symbol(symbol_table, syntax->array_variable->name);

            
            break;
        }
        case STRUCT_DECLARATION:
        {
            // no support for struct in intermediate code generation
            break;
        }
        case STRUCT_VARIABLE:
        {
            // no support for struct in intermediate code generation
            break;
        }
        case IF_STATEMENT:
        {
            // TODO
            //semantic_analysis(syntax->if_statement->condition)
               
            ENTER_SCOPE();
            //semantic_analysis(syntax->if_statement->body)
            LEAVE_SCOPE();
            break;
        }
        case RETURN_STATEMENT:
        {
            Quad * quad = quad_new(OP_RETURN);
            generate_expression(code_list, syntax->return_statement->expression, quad->result);
            list_append(code_list, quad);
            break;
        }
        case FUNCTION_DECLARATION:
        {
            Symbol * previous_symbol = get_symbol(symbol_table, syntax->function_declaration->name);
         
            if(previous_symbol == NULL)
            {
                Symbol *symbol = symbol_new();
                symbol->level = cur_level;
                strcpy(symbol->name, syntax->function_declaration->name);
                symbol->declaration = syntax;
                insert_symbol(symbol_table, symbol);
            }

            Quad * quad = quad_new(OP_FUNCTION);
            strcpy(quad->result, syntax->function_declaration->name);
            list_append(code_list, quad);

            if(syntax->function_declaration->arguments != NULL)
            {
                for(int i = 0; i < list_length(syntax->function_declaration->arguments->block->statements); ++i)
                {
                    Syntax * argument = (Syntax *)list_get(syntax->function_declaration->arguments->block->statements, i);
                    Quad * quad = quad_new(OP_PARAM);
                    if(argument->type == VARIABLE_DECLARATION)
                        strcpy(quad->result, argument->variable_declaration->name);
                    else if(argument->type == ARRAY_DECLARATION)
                        strcpy(quad->result, argument->array_declaration->name);
                    list_append(code_list, quad);
                }
            }
            

            
            ENTER_SCOPE();
            old_function = cur_function;
            cur_function = syntax;

            generate_intermediate_code(code_list, syntax->function_declaration->block);
               
            LEAVE_SCOPE();

            cur_function = old_function;
            break;
        }
        case FUNCTION_CALL:
        {
            Symbol * previous_symbol = get_symbol(symbol_table, syntax->function_call->name);
            // TODO
            
            break;
        }
        case ASSIGNMENT:
        {
            // TODO
            break;
        }
        case WHILE_STATEMENT:
        {
            // TODO
            //semantic_analysis(syntax->while_statement->condition)
           
            ENTER_SCOPE();
            //semantic_analysis(syntax->while_statement->body)
                
            LEAVE_SCOPE();
            break;
        }
        case BLOCK:
        {
            for(int i = 0; i < list_length(syntax->block->statements); ++i)
                generate_intermediate_code(code_list, (Syntax *)list_get(syntax->block->statements, i));

            break;
        }
        case TOP_LEVEL:
        {
            // initialize the symbol table
            symbol_table = new_symbol_table();  
            cur_function = syntax;

            for(int i = 0; i < list_length(syntax->top_level->statements); ++i)
                generate_intermediate_code(code_list, (Syntax *)list_get(syntax->top_level->statements, i));

            break;
        }
        default: printf("Error!Undefined type %d!\n", syntax->type); break;
    }
}

Quad * quad_new(Operator op)
{
    Quad * quad = (Quad *)malloc(sizeof(Quad));
    quad->op = op;
    quad->arg1 = string_new(33);
    quad->arg2 = string_new(33);
    quad->result = string_new(33);
    return quad;
}

void quad_delete(Quad * quad)
{
    free(quad->arg1);
    free(quad->arg2);
    free(quad->result);
    free(quad);
}

void new_label(char *name)
{
    static int number = 0;
    sprintf(name, "Label%d", number);
    ++number;
}

void new_temp(char *name)
{
    static int number = 0;
    sprintf(name, "t%d", number);
    ++number;
}

void print_quad(Quad * quad)
{
    printf("(%d, %s, %s, %s)\n", quad->op, quad->arg1, quad->arg2, quad->result);
}

void print_quad_list(List *code_list)
{
    for(int i = 0; i < list_length(code_list); ++i)
    {
        Quad * quad = (Quad *)list_get(code_list, i);
        print_quad(quad);
    }
}