#include <assert.h>
#include "targetcode.h"
#include "symboltable.h"

extern SymbolTable *symbol_table;


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

void load_to_register(FILE * out, char * name)
{

}

void allocate_address(char * name, int address)
{
    Symbol * symbol = symbol_new();
    strcpy(symbol->name, name);
    symbol->address = address;
}

int allocate_addresses(List * code_list)
{
    int total_size = 0;
    for(int i = 0; i < list_length(code_list); ++i)
    {
        Quad *quad = (Quad *)list_get(code_list, i);
        switch(quad->op)
        {
            case OP_DEFINE:
            {
                allocate_address()
            }
        }
    }
    
}

void translate_quad(FILE * out, Quad * quad)
{

}

void translate_function_code(FILE * out, List * code_list)
{
    symbol_table = new_symbol_table();

    Quad * function_quad = (Quad *)list_get(code_list, 0);
    assert(function_quad->op == OP_FUNCTION);

    // write function header
    int total_size = allocate_address(code_list, symbol_table);
    fprintf(out, "%s:\n", function_quad->result);
    fprintf(out, "addi $sp, $sp, -%d", total_size);
    for(int i = 1; i < list_length(code_list); ++i)
        translate_quad(out, (Quad *)list_get(code_list, i));

    // write function tail
    fprintf(out, "addi $sp, $sp, %d", total_size);

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