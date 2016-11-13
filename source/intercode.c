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
            Symbol * previous_symbol = get_symbol(symbol_table, syntax->variable->name);
            
            assert(previous_symbol != NULL);
            strcpy(temp, previous_symbol->var_name);

            break;
        }
        case FUNCTION_CALL:
        {
            List * arg_list = list_new();
            if(syntax->function_call->arguments != NULL)
            {
                for(int i = 0; i < list_length(syntax->function_call->arguments->block->statements); ++i)
                {
                    Syntax * argument = (Syntax *)list_get(syntax->function_call->arguments->block->statements, i);
                    char result_temp[5];
                    generate_expression(code_list, argument, result_temp);
                    Quad * quad = quad_new(OP_ARG);
                    strcpy(quad->result, result_temp);
                    list_append(arg_list, quad);
                }
            }

            list_append_list(code_list, arg_list);
            list_delete(arg_list);

            Quad * quad = quad_new(OP_CALL);
            new_temp(quad->result);
            strcpy(quad->arg1, syntax->function_call->name);
            list_append(code_list, quad);

            strcpy(temp, quad->result);

            break;
        }
        case IMMEDIATE:
        {
            sprintf(temp, "%d", syntax->immediate->int_value);   
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
            
            assert(previous_symbol == NULL);

            Symbol *symbol = symbol_new();
            symbol->level = cur_level;
            strcpy(symbol->name, syntax->variable_declaration->name);
            new_variable(symbol->var_name);
            symbol->declaration = syntax;
            insert_symbol(symbol_table, symbol);
            
            break;
        }
        case ARRAY_DECLARATION:
        {
            Symbol * previous_symbol = get_symbol(symbol_table, syntax->array_declaration->name);

            break;
        }
        case STRUCT_DECLARATION:
        {
            // no support for struct in intermediate code generation
            break;
        }
        case IF_STATEMENT:
        {
            assert(syntax->if_statement->condition->type == BINARY_EXPRESSION);
            char left_temp[5];
            char right_temp[5];
            generate_expression(code_list, syntax->if_statement->condition->binary_expression->left, left_temp);
            generate_expression(code_list, syntax->if_statement->condition->binary_expression->right, right_temp);
            
            char success_label[10];
            char failure_label[10];
            new_label(success_label);
            new_label(failure_label);

            // if code
            Quad * quad = quad_new(binary_type_to_operator(syntax->if_statement->condition->binary_expression->type));
            strcpy(quad->arg1, left_temp);
            strcpy(quad->arg2, right_temp);
            strcpy(quad->result, success_label);
            list_append(code_list, quad);

            // goto failure label if failed
            quad = quad_new(OP_GOTO);
            strcpy(quad->result, failure_label);
            list_append(code_list, quad);

            // success label
            quad = quad_new(OP_LABEL);
            strcpy(quad->result, success_label);
            list_append(code_list, quad);
               
            ENTER_SCOPE();
            generate_intermediate_code(code_list, syntax->if_statement->then_body);
            quad = quad_new(OP_GOTO);
            strcpy(quad->result, failure_label);
            list_append(code_list, quad);
            LEAVE_SCOPE();

            quad = quad_new(OP_LABEL);
            strcpy(quad->result, failure_label);
            list_append(code_list, quad);

            ENTER_SCOPE();
            generate_intermediate_code(code_list, syntax->if_statement->else_body);
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
            Quad * quad = quad_new(OP_FUNCTION);
            strcpy(quad->result, syntax->function_declaration->name);
            list_append(code_list, quad);

            ENTER_SCOPE();
            old_function = cur_function;
            cur_function = syntax;

            // process the arguments
            if(syntax->function_declaration->arguments != NULL)
            {
                for(int i = 0; i < list_length(syntax->function_declaration->arguments->block->statements); ++i)
                {
                    Syntax * argument = (Syntax *)list_get(syntax->function_declaration->arguments->block->statements, i);
                    
                    // insert into symbol table
                    Symbol *symbol = symbol_new();
                    symbol->level = cur_level;
                    strcpy(symbol->name, argument->variable_declaration->name);
                    new_variable(symbol->var_name);
                    symbol->declaration = argument;
                    insert_symbol(symbol_table, symbol);

                    // add PARAM code
                    Quad * quad = quad_new(OP_PARAM);
                    strcpy(quad->result, symbol->var_name);
                    list_append(code_list, quad);
                }
            }

            generate_intermediate_code(code_list, syntax->function_declaration->block);
               
            LEAVE_SCOPE();

            cur_function = old_function;
            break;
        }
        case FUNCTION_CALL:
        {
            Symbol * previous_symbol = get_symbol(symbol_table, syntax->function_call->name);
            
            char result_temp[5];
            generate_expression(code_list, syntax, result_temp);

            break;
        }
        case ASSIGNMENT:
        {
            char dest_temp[5];
            char result_temp[5];
            generate_expression(code_list, syntax->assignment->dest, dest_temp);
            generate_expression(code_list, syntax->assignment->expression, result_temp);
            Quad *quad = quad_new(OP_DEFINE);
            strcpy(quad->arg1, result_temp);
            strcpy(quad->result, dest_temp);
            list_append(code_list, quad);
            break;
        }
        case WHILE_STATEMENT:
        {
            assert(syntax->while_statement->condition->type == BINARY_EXPRESSION);

            char start_label[10];
            char success_label[10];
            char failure_label[10];
            new_label(start_label);
            new_label(success_label);
            new_label(failure_label);

            Quad *quad = quad_new(OP_LABEL);
            strcpy(quad->result, start_label);
            list_append(code_list, quad);

            // do check the condition
            char left_temp[5];
            char right_temp[5];
            generate_expression(code_list, syntax->while_statement->condition->binary_expression->left, left_temp);
            generate_expression(code_list, syntax->while_statement->condition->binary_expression->right, right_temp);
            
            quad = quad_new(binary_type_to_operator(syntax->while_statement->condition->binary_expression->type));
            strcpy(quad->arg1, left_temp);
            strcpy(quad->arg2, right_temp);
            strcpy(quad->result, success_label);
            list_append(code_list, quad);

            quad = quad_new(OP_LABEL);
            strcpy(quad->result, success_label);
            list_append(code_list, quad);

            ENTER_SCOPE();
            generate_intermediate_code(code_list, syntax->while_statement->body);
            LEAVE_SCOPE();

            quad = quad_new(OP_GOTO);
            strcpy(quad->result, start_label);
            list_append(code_list, quad);

            quad = quad_new(OP_LABEL);
            strcpy(quad->result, failure_label);
            list_append(code_list, quad);
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
    static int number = 1;
    sprintf(name, "Label%d", number);
    ++number;
}

void new_temp(char *name)
{
    static int number = 1;
    sprintf(name, "t%d", number);
    ++number;
}

void new_variable(char *name)
{
    static int number = 1;
    sprintf(name, "v%d", number);
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
        printf("%d: ", i + 1);
        print_quad(quad);
    }
}