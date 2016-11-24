#include <assert.h>
#include <ctype.h>
#include "targetcode.h"
#include "symboltable.h"

static SymbolTable *symbol_table;

void generate_header(FILE * out)
{
    fprintf(out, "%s\n",".data");
	fprintf(out, "%s\n","_prompt: .asciiz \"Enter an integer:\"");
	fprintf(out, "%s\n","_ret: .asciiz \"\\n\"");
	fprintf(out, "%s\n",".globl main");
	fprintf(out, "%s\n",".text");
	fprintf(out, "%s\n","readint:");
	fprintf(out, "%s\n","li $v0, 4");
	fprintf(out, "%s\n","la $a0, _prompt");
	fprintf(out, "%s\n","syscall");
	fprintf(out, "%s\n","li $v0, 5");
	fprintf(out, "%s\n","syscall");
	fprintf(out, "%s\n","jr $ra");
	fprintf(out, "%s\n","writeint:");
	fprintf(out, "%s\n","li $v0, 1");
	fprintf(out, "%s\n","syscall");
	fprintf(out, "%s\n","li $v0, 4");
	fprintf(out, "%s\n","la $a0, _ret");
	fprintf(out, "%s\n","syscall");
	fprintf(out, "%s\n","move $v0, $0");
	fprintf(out, "%s\n","jr $ra");
}

int get_variable_address(char *name)
{
    Symbol * symbol = get_symbol(symbol_table, name);
    assert(symbol != NULL);
    return symbol->address;
}


void load_to_register(FILE * out, char * name, int register_num)
{
    Symbol * previous_symbol = get_symbol(symbol_table, name);
    assert(previous_symbol != NULL);

    char register_name[5];
    sprintf(register_name, "$t%d", register_num);
    if(isdigit(name[0]))
        fprintf(out, "li %s, %s\n", register_name, &name[0]);
    else
        fprintf(out, "lw %s, %d($sp)\n", register_name, get_variable_address(name));
}


int allocate_addresses(List * code_list)
{
    #define ALLOCATE_ADDRESS(var_name, size) \
    { \
        if(get_symbol(symbol_table, var_name) == NULL)\
        {\
            Symbol * symbol = symbol_new();\
            strcpy(symbol->name, (var_name));\
            symbol->address = total_size;\
            insert_symbol(symbol_table, symbol); \
            total_size += (size);\
        }\
    }

    int total_size = 0;
    for(int i = 0; i < list_length(code_list); ++i)
    {
        Quad *quad = (Quad *)list_get(code_list, i);
        switch(quad->op)
        {
            case OP_RETURN:
            {
                if(quad->result[0] != '\0')
                    ALLOCATE_ADDRESS(quad->result, 4);
                break;
            }
            case OP_DEFINE:
            {
                ALLOCATE_ADDRESS(quad->arg1, 4);
                ALLOCATE_ADDRESS(quad->result, 4);
                break;
            }
            case OP_PARAM:
            {
                ALLOCATE_ADDRESS(quad->result, 4);
                break;
            }
            case OP_CALL:
            {
                ALLOCATE_ADDRESS(quad->result, 4);
                break;
            }
            case OP_PLUS: case OP_MINUS: case OP_MULTIPLY: case OP_DIVIDE:
            {
                ALLOCATE_ADDRESS(quad->arg1, 4);
                ALLOCATE_ADDRESS(quad->arg2, 4);
                ALLOCATE_ADDRESS(quad->result, 4);
                break;
            }
            case OP_BITWISE_NEGATION: case OP_LOGICAL_NEGATION:
            {
                ALLOCATE_ADDRESS(quad->arg1, 4);
                ALLOCATE_ADDRESS(quad->result, 4);
                break;
            }
            case OP_LESS: case OP_GREATER: case OP_EQUAL: case OP_NOT_EQUAL: case OP_LESS_OR_EQUAL: case OP_GREATER_OR_EQUAL:
            {
                ALLOCATE_ADDRESS(quad->arg1, 4);
                ALLOCATE_ADDRESS(quad->arg2, 4);
                break;
            }
            default: break;
        }
    }
    return total_size;
}


void translate_function_code(FILE * out, List * code_list)
{
    symbol_table = symbol_table_new();
    // traverse the code list to reserve the space for variables
    int function_stack_size = allocate_addresses(code_list);

    Quad * function_quad = (Quad *)list_get(code_list, 0);
    assert(function_quad->op == OP_FUNCTION);

    // write function header
    fprintf(out, "%s:\n", function_quad->result);
    fprintf(out, "addi $sp, $sp, -%d\n", function_stack_size);

    // traverse and translate quad one by one
    for(int i = 1; i < list_length(code_list); ++i)
    {
        Quad *quad = (Quad *)list_get(code_list, i);
        switch(quad->op)
        {
            case OP_GOTO: 
            {
                fprintf(out, "j %s\n", quad->result);
                break;
            }
            case OP_RETURN:
            {
                if(quad->result[0] != '\0')
                {
                    load_to_register(out, quad->result, 0);
                    fprintf(out, "move $v0, $t0\n");
                }
                fprintf(out, "addi $sp, $sp, %d\n", function_stack_size);
                fprintf(out, "jr $ra\n");
                break;
            }
            case OP_ARG:
            {
                int arg_count = 0;
                while(quad->op == OP_ARG)
                {
                    load_to_register(out, quad->result, 0);
                    fprintf(out, "move $a%d, $t0\n", arg_count);
                    ++i;
                    ++arg_count;
                    quad = list_get(code_list, i);
                }
                --i;
                break;
            }
            case OP_PARAM: 
            {
                int param_count = 0;
                while(quad->op == OP_PARAM)
                {
                    fprintf(out, "sw $a%d, %d($sp)\n", param_count, get_variable_address(quad->result));
                    ++i;
                    ++param_count;
                    quad = list_get(code_list, i);
                }
                --i;
                break;
            }
            case OP_LABEL:
            {
                fprintf(out, "%s:\n", quad->result);
                break;
            }
            case OP_DEFINE: 
            {
                load_to_register(out, quad->arg1, 0);
                fprintf(out, "sw $t0, %d($sp)\n", get_variable_address(quad->result));
                break;
            } 
            case OP_CALL: 
            {
                fprintf(out, "addi $sp, $sp, -4\n");
                fprintf(out, "sw $ra, 0($sp)\n");
                fprintf(out, "jal %s\n", quad->arg1);
                fprintf(out, "lw $ra, 0($sp)\n");
                fprintf(out, "addi $sp, $sp, 4\n");
                fprintf(out, "sw $v0, %d($sp)\n", get_variable_address(quad->result));
                break;
            }
            case OP_PLUS: 
            {
                load_to_register(out, quad->arg1, 0);
                load_to_register(out, quad->arg2, 1);
                fprintf(out, "add $t0, $t0, $t1\n");
                fprintf(out, "sw $t0, %d($sp)\n", get_variable_address(quad->result));
                break;
            }
            case OP_MINUS: 
            {
                load_to_register(out, quad->arg1, 0);
                load_to_register(out, quad->arg2, 1);
                fprintf(out, "sub $t0, $t0, $t1\n");
                fprintf(out, "sw $t0, %d($sp)\n", get_variable_address(quad->result));
                break;
            }
            case OP_MULTIPLY:
            {
                load_to_register(out, quad->arg1, 0);
                load_to_register(out, quad->arg2, 1);
                fprintf(out, "mul $t0, $t0, $t1\n");
                fprintf(out, "sw $t0, %d($sp)\n", get_variable_address(quad->result));
                break;
            }
            case OP_DIVIDE: 
            {
                load_to_register(out, quad->arg1, 0);
                load_to_register(out, quad->arg2, 1);
                fprintf(out, "div $t0, $t0, $t1\n");
                fprintf(out, "mflo $t0\n");
                fprintf(out, "sw $t0, %d($sp)\n", get_variable_address(quad->result));
                break;
            }
            case OP_LESS: 
            {
                load_to_register(out, quad->arg1, 0);
                load_to_register(out, quad->arg2, 1);
                fprintf(out, "%s $t0,$t1,%s\n", "blt", quad->result);
                break;
            } 
                
            case OP_GREATER: 
            {
                load_to_register(out, quad->arg1, 0);
                load_to_register(out, quad->arg2, 1);
                fprintf(out, "%s $t0,$t1,%s\n", "bgt", quad->result);
                break;
            }
            case OP_EQUAL:
            {
                load_to_register(out, quad->arg1, 0);
                load_to_register(out, quad->arg2, 1);
                fprintf(out, "%s $t0,$t1,%s\n", "beq", quad->result);
                break;
            }
            case OP_NOT_EQUAL:
            {
                load_to_register(out, quad->arg1, 0);
                load_to_register(out, quad->arg2, 1);
                fprintf(out, "%s $t0,$t1,%s\n", "bne", quad->result);
                break;
            }
            case OP_LESS_OR_EQUAL: 
            {
                load_to_register(out, quad->arg1, 0);
                load_to_register(out, quad->arg2, 1);
                fprintf(out, "%s $t0,$t1,%s\n", "ble", quad->result);
                break;
            }
            case OP_GREATER_OR_EQUAL:
            {
                load_to_register(out, quad->arg1, 0);
                load_to_register(out, quad->arg2, 1);
                fprintf(out, "%s $t0,$t1,%s\n", "bge", quad->result);
                break;
            }
            default: break;
        }
    }

    // write function tail
    //fprintf(out, "addi $sp, $sp, %d\n", function_stack_size);

    symbol_table_delete(symbol_table);
    symbol_table = NULL;
}

void generate_target_code(FILE * out, List * code_list)
{
    generate_header(out);

    // divide all function scopes and generate corresponding target code
    List * function_code_list = list_new();
    for(int i = 0; i < list_length(code_list); ++i)
    {
        Quad *quad = (Quad *)list_get(code_list, i);
        if(quad->op == OP_FUNCTION || i == list_length(code_list) - 1)
        {
            if(list_length(function_code_list) != 0)
            {
                translate_function_code(out, function_code_list);
                list_delete(function_code_list);
                function_code_list = list_new();
            }
        }
        list_append(function_code_list, quad);
    }
}