#include <assert.h>
#include "commonutils.h"
#include "semantic.h"

#define ENTER_SCOPE(x) { old_scope = cur_scope; cur_level++; cur_scope = x; }
#define LEAVE_SCOPE() { cur_scope = old_scope; remove_level(symbol_table, cur_level); cur_level--; }

SymbolTable * symbol_table = NULL;

void variable_type_to_string(Syntax * type, char * type_string)
{
    assert(type->type == VARIABLE_TYPE);
    char buf[50];
    if(type->variable_type->type == INT)
        strcpy(type_string, "int");
    else if(type->variable_type->type == FLOAT)
        strcpy(type_string, "float");
    else if(type->variable_type->type == STRUCT)
        sprintf(type_string, "struct %s", type->variable_type->name);
}

bool is_variable_type_equal(Syntax * type1, Syntax * type2)
{
    assert(type1 != NULL && type2 != NULL);
    assert(type1->type == VARIABLE_TYPE && type2->type == VARIABLE_TYPE);
    bool is_equal = true;
    if(type1->variable_type->type != type2->variable_type->type)
        is_equal = false; 
    else if(type1->variable_type->type == STRUCT && strcmp(type1->variable_type->name, type2->variable_type->name) != 0)
        is_equal = false;
    return is_equal;
}

Syntax * check_expression_type(Syntax * syntax)
{
    assert(syntax != NULL);
    switch(syntax->type)
    {
        case IMMEDIATE:
        {
            // wrap the immediate's type
            Syntax * type = syntax_new(VARIABLE_TYPE);
            type->variable_type->type = syntax->immediate->type;
            return type;
        }
        case VARIABLE:
        {
            semantic_analysis(syntax);
            Symbol * previous_symbol = get_symbol(symbol_table, syntax->variable->name);
            if(previous_symbol != NULL)
                return previous_symbol->declaration->variable_declaration->type;
        }
        case ARRAY_VARIABLE:
        {
            semantic_analysis(syntax);
            Symbol * previous_symbol = get_symbol(symbol_table, syntax->array_variable->name);
            if(previous_symbol != NULL)
                return previous_symbol->declaration->array_declaration->type;
        }
        case STRUCT_VARIABLE:
        {
            semantic_analysis(syntax);
            char struct_name[50];
            sprintf(struct_name, "struct %s", syntax->struct_variable->name);
            Symbol * previous_symbol = get_symbol(symbol_table, struct_name);
            assert(previous_symbol != NULL);
            Syntax * block = previous_symbol->declaration->struct_declaration->block;
            for(int i = 0; i < list_length(block->block->statements) - 1; ++i)
            {
                Syntax * declaration = list_get(block->block->statements, i);
                if(declaration->type == VARIABLE_DECLARATION)
                {
                    if(strcmp(declaration->variable_declaration->name, syntax->struct_variable->member) == 0)
                    return declaration->variable_declaration->type;
                }
                else if(declaration->type == ARRAY_DECLARATION)
                {
                    if(strcmp(declaration->array_declaration->name, syntax->struct_variable->member) == 0)
                    return declaration->array_declaration->type;
                }
            }
            break;
        }
        case BINARY_EXPRESSION:
        {
            Syntax * type1 = check_expression_type(syntax->binary_expression->left);
            Syntax * type2 = check_expression_type(syntax->binary_expression->right);

    printf("dsadsada %d dsadsdsa\n", syntax->binary_expression->type);
            if(type1 != NULL && type2 != NULL)
            {
                if(!is_variable_type_equal(type1, type2))
                {
                    char buf[50];
                    sprintf(buf, "Expression type conflicts.");
                    print_error(buf, NULL, syntax->lineno);   
                }
                else
                    return type1;
            }
        }
        case UNARY_EXPRESSION:
        {
            return check_expression_type(syntax->unary_expression->expression);
        }
        default: break;
    }
    return NULL;
}

bool semantic_analysis(Syntax * syntax)
{
    static int cur_level = 0;
    static Syntax * cur_scope = NULL;
    static Syntax * old_scope = NULL;
    static bool is_correct = true;

    if(syntax == NULL)
    {
        printf("(null)\n");
        return false;
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
            // redefinition check
            else
            {
                char buf[50];
                sprintf(buf, "Redefinition of '%s'", syntax->variable_declaration->name);
                print_error(buf, syntax->variable_declaration->name, syntax->lineno);
                print_note("Previous definition is here", syntax->variable_declaration->name, previous_symbol->declaration->lineno);
                is_correct = false;
            }
            // incomplete struct type check
            if(syntax->variable_declaration->type->variable_type->type == STRUCT)
            {
                char struct_name[50];
                sprintf(struct_name, "struct %s", syntax->variable_declaration->type->variable_type->name);
                
                Symbol * struct_symbol = get_symbol(symbol_table, struct_name);
                if(struct_symbol == NULL)
                {
                    char buf[50];
                    sprintf(buf, "Variable has incomplete type '%s'", struct_name);
                    print_error(buf, struct_name, syntax->lineno);
                    is_correct = false;
                }
            }
            break;
        }
        case VARIABLE:
        {
            Symbol * previous_symbol = get_symbol(symbol_table, syntax->variable->name);
            
            // undefined variable check
            if(previous_symbol == NULL)
            {
                char buf[50];
                sprintf(buf, "Use of undeclared identifier '%s'", syntax->variable->name);
                print_error(buf, syntax->variable->name, syntax->lineno);
                is_correct = false;
            }
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
            else
            {
                char buf[50];
                sprintf(buf, "Redefinition of '%s'", syntax->array_declaration->name);
                print_error(buf, syntax->array_declaration->name, syntax->lineno);
                print_note("Previous definition is here", syntax->array_declaration->name, previous_symbol->declaration->lineno);
                is_correct = false;
            }
            break;
        }
        case ARRAY_VARIABLE:
        {
            Symbol * previous_symbol = get_symbol(symbol_table, syntax->array_variable->name);

            // undefined variable check
            if(previous_symbol == NULL)
            {
                char buf[50];
                sprintf(buf, "Use of undeclared identifier '%s'", syntax->array_variable->name);
                print_error(buf, syntax->array_variable->name, syntax->lineno);
                is_correct = false;
            }
            else
            {
                // '[]' used on non-array variable
                if(previous_symbol->declaration->type != ARRAY_DECLARATION)
                {
                    char buf[50];
                    sprintf(buf, "'[]' used on non-array variable '%s'", syntax->array_variable->name);
                    print_error(buf, syntax->array_variable->name, syntax->lineno);
                    is_correct = false;
                }
            }
            break;
        }
        case STRUCT_DECLARATION:
        {
            char struct_name[50];
            sprintf(struct_name, "struct %s", syntax->struct_declaration->name);

            Symbol * previous_symbol = get_symbol(symbol_table, struct_name);
            // redefinition check
            if(previous_symbol == NULL)
            {
                Symbol *symbol = symbol_new();
                symbol->level = cur_level;
                strcpy(symbol->name, struct_name);
                symbol->declaration = syntax;
                insert_symbol(symbol_table, symbol);
            }
            else
            {
                char buf[50];
                sprintf(buf, "Redefinition of '%s'", struct_name);
                print_error(buf, struct_name, syntax->lineno);
                print_note("Previous definition is here", syntax->struct_declaration->name, previous_symbol->declaration->lineno);
            }
            ENTER_SCOPE(syntax);
            semantic_analysis(syntax->struct_declaration->block);
            LEAVE_SCOPE();
            break;
        }
        case STRUCT_VARIABLE:
        {
            Symbol * previous_symbol = get_symbol(symbol_table, syntax->struct_variable->name);
            
            // undefined variable check
            if(previous_symbol == NULL)
            {
                char buf[50];
                sprintf(buf, "Use of undeclared identifier '%s'", syntax->struct_variable->name);
                print_error(buf, syntax->struct_variable->name, syntax->lineno);
                is_correct = false;
            }
            else
            {
                // illegal member access
                if(previous_symbol->declaration->type == VARIABLE_DECLARATION && 
                   previous_symbol->declaration->variable_declaration->type->variable_type->type == STRUCT)
                {
                    char struct_name[50];
                    sprintf(struct_name, "struct %s", previous_symbol->declaration->variable_declaration->type->variable_type->name);
                    Symbol * struct_symbol = get_symbol(symbol_table, struct_name);
                    
                    // if the variable's type is incomplete, abandon the check
                    if(struct_symbol == NULL)
                        break;

                    Syntax * block = struct_symbol->declaration->struct_declaration->block;
                    bool has_member = false;
                    for(int i = 0; i < list_length(block->block->statements); ++i)
                    {
                        Syntax * member = list_get(block->block->statements, i);
                        if((member->type == VARIABLE_DECLARATION && strcmp(member->variable_declaration->name, syntax->struct_variable->member) == 0) ||
                           (member->type == ARRAY_DECLARATION && strcmp(member->array_declaration->name, syntax->struct_variable->member) == 0))
                        {
                            has_member = true;
                            break;
                        }
                    }
                    if(!has_member)
                    {
                        char buf[50];
                        sprintf(buf, "Illegal member access of '%s.%s'", syntax->struct_variable->name, syntax->struct_variable->member);
                        print_error(buf, syntax->struct_variable->member, syntax->lineno);
                        is_correct = false;
                    }
                }
                // '.' used on non-struct variable
                else
                {
                    char buf[50];
                    sprintf(buf, "'.' used on non-struct variable '%s'", syntax->struct_variable->name);
                    print_error(buf, syntax->struct_variable->name, syntax->lineno);
                    is_correct = false;
                }
            }
            break;
        }
        case UNARY_EXPRESSION:
        {
            // type conflicts
            check_expression_type(syntax);
            break;
        }
        case BINARY_EXPRESSION:
        {
            // type conflicts
            check_expression_type(syntax);
            break;
        }
        case IF_STATEMENT:
        {
            // condition type
            semantic_analysis(syntax->if_statement->condition);
            ENTER_SCOPE(syntax);
            semantic_analysis(syntax->if_statement->body);
            LEAVE_SCOPE();
            break;
        }
        case RETURN_STATEMENT:
        {
            // return type doesn't match the function type
            Syntax * type = check_expression_type(syntax->return_statement->expression);
            if(type != NULL)
            {
                if(!is_variable_type_equal(cur_scope->function_declaration->type, type))
                {
                    char buf[50];
                    char return_type[50];
                    char function_type[50];
                    variable_type_to_string(type, return_type);
                    variable_type_to_string(cur_scope->function_declaration->type, function_type);
                    sprintf(buf, "Return type %s doesn't match the function type %s'", return_type, function_type);
                    print_error(buf, "return", syntax->lineno);
                    is_correct = false;
                }
            }
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
            // redefinition check
            else
            {
                char buf[50];
                sprintf(buf, "Redefinition of '%s'", syntax->function_declaration->name);
                print_error(buf, syntax->function_declaration->name, syntax->lineno);
                print_note("Previous definition is here", syntax->function_declaration->name, previous_symbol->declaration->lineno);
                is_correct = false;
            }
            ENTER_SCOPE(syntax);
            semantic_analysis(syntax->function_declaration->arguments);
            semantic_analysis(syntax->function_declaration->block);
            LEAVE_SCOPE();
            break;
        }
        case FUNCTION_CALL:
        {
            Symbol * previous_symbol = get_symbol(symbol_table, syntax->function_call->name);

            // undefined function call
            if(previous_symbol == NULL)
            {
                char buf[50];
                sprintf(buf, "Undefined function name '%s'", syntax->function_call->name);
                print_error(buf, syntax->function_call->name, syntax->lineno);
                is_correct = false;
            }
            // '()' used on non-function
            else if(previous_symbol->declaration->type != FUNCTION_DECLARATION)
            {
                char buf[50];
                sprintf(buf, "'()' used on non-function '%s'", syntax->function_call->name);
                print_error(buf, syntax->function_call->name, syntax->lineno);
                is_correct = false;
            }
            // arguments don't match

            //semantic_analysis(syntax->function_call->arguments);
            break;
        }
        case ASSIGNMENT:
        {
            semantic_analysis(syntax->assignment->dest);
            // inconsistent type
            Syntax * expression_type = check_expression_type(syntax->assignment->expression);
            Syntax * dest_type = check_expression_type(syntax->assignment->dest);
            if(!is_variable_type_equal(expression_type, dest_type))
            {
                char buf[50];
                sprintf(buf, "Assignment has inconsistent types");
                print_error(buf, "=", syntax->lineno);
                is_correct = false;
            }
            break;
        }
        case WHILE_STATEMENT:
        {
            semantic_analysis(syntax->while_statement->condition);

            // condition type check

            ENTER_SCOPE(syntax);
            semantic_analysis(syntax->while_statement->body);
            LEAVE_SCOPE();
            break;
        }
        case BLOCK:
        {
            for(int i = 0; i < list_length(syntax->block->statements); ++i)
                semantic_analysis((Syntax *)list_get(syntax->block->statements, i));
            break;
        }
        case TOP_LEVEL:
        {
            // initialize the symbol table
            symbol_table = new_symbol_table();  
            cur_scope = syntax;

            printf("TopLevel\n");
            for(int i = 0; i < list_length(syntax->top_level->statements); ++i)
                semantic_analysis((Syntax *)list_get(syntax->top_level->statements, i));
            
            break;
        }
        default: printf("Error!Undefined type!\n"); break;
    }
    return is_correct;
}