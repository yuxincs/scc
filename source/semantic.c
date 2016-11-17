#include <assert.h>
#include "commonutils.h"
#include "semantic.h"

#define ENTER_SCOPE() { cur_level++; }

#define LEAVE_SCOPE() { remove_level(symbol_table, cur_level); cur_level--; }

SymbolTable * symbol_table = NULL;

void variable_type_to_string(Syntax * type, char * type_string)
{
    assert(type->type == VARIABLE_TYPE);
    char buf[100];
    if(type->variable_type->type == INT)
        strcpy(type_string, "int");
    else if(type->variable_type->type == FLOAT)
        strcpy(type_string, "float");
    else if(type->variable_type->type == STRUCT)
        sprintf(type_string, "struct %s", type->variable_type->name);
    else if(type->variable_type->type == VOID)
        strcpy(type_string, "void");
}

bool is_variable_type_equal(Syntax * type1, Syntax * type2)
{
    if(type1 == NULL || type2 == NULL)
        return false;

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
    // TODO:
    // the wrapped variable type syntax will not be deleted
    // after usage, which will cause memory leaks
    if(syntax == NULL)
    {
        // wrap the void type
        Syntax * type = syntax_new(VARIABLE_TYPE);
        type->variable_type->type = VOID;
        return type;
    }
    switch(syntax->type)
    {
        case IMMEDIATE:
        {
            // wrap the immediate's type
            Syntax * type = syntax_new(VARIABLE_TYPE);
            type->variable_type->type = IMMEDIATE;
            return type;
        }
        case VARIABLE:
        {
            if(semantic_analysis(syntax))
            {
                Symbol * previous_symbol = get_symbol(symbol_table, syntax->variable->name);
                if(previous_symbol != NULL)
                    return previous_symbol->declaration->variable_declaration->type;
            }
            break;
        }
        case ARRAY_VARIABLE:
        {
            if(semantic_analysis(syntax))
            {
                Symbol * previous_symbol = get_symbol(symbol_table, syntax->array_variable->name);
                if(previous_symbol != NULL)
                    return previous_symbol->declaration->array_declaration->type;
            }
            break;
        }
        case STRUCT_VARIABLE:
        {
            if(semantic_analysis(syntax))
            {
                Symbol * previous_symbol = get_symbol(symbol_table, syntax->struct_variable->name);

                assert(previous_symbol != NULL);
                char struct_name[100];
                sprintf(struct_name, "struct %s", previous_symbol->declaration->variable_declaration->type->variable_type->name);
                Symbol * struct_symbol = get_symbol(symbol_table, struct_name);
                assert(struct_symbol != NULL);

                Syntax * block = struct_symbol->declaration->struct_declaration->block;
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
            }
            break;
        }
        case FUNCTION_CALL:
        {
            if(semantic_analysis(syntax))
            {
                Symbol * previous_symbol = get_symbol(symbol_table, syntax->function_call->name);
                assert(previous_symbol != NULL);
                return previous_symbol->declaration->function_declaration->type;
            }
            
            break;
        }
        case BINARY_EXPRESSION:
        {
            Syntax * type1 = check_expression_type(syntax->binary_expression->left);
            Syntax * type2 = check_expression_type(syntax->binary_expression->right);
   
            if(type1 != NULL && type2 != NULL)
            {
                if(!is_variable_type_equal(type1, type2))
                {
                    char buf[100];
                    char type1_str[100];
                    char type2_str[100];
                    variable_type_to_string(type1, type1_str);
                    variable_type_to_string(type2, type2_str);
                    sprintf(buf, "Expression type conflicts, expected '%s', has '%s'", type1_str, type2_str);
                    print_error(buf, NULL, syntax->lineno);
                }
                else
                    return type1;
            }
            break;
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
    static Syntax * cur_function = NULL;
    static Syntax * old_function = NULL;
    bool is_correct = true;

    if(syntax == NULL)
        return true;

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
            else if(previous_symbol->level == cur_level)
            {
                char buf[100];
                sprintf(buf, "Redefinition of '%s'", syntax->variable_declaration->name);
                print_error(buf, syntax->variable_declaration->name, syntax->lineno);
                print_note("Previous definition is here", syntax->variable_declaration->name, previous_symbol->declaration->lineno);
                is_correct = false;
            }
            // incomplete struct type check
            if(syntax->variable_declaration->type->variable_type->type == STRUCT)
            {
                char struct_name[100];
                sprintf(struct_name, "struct %s", syntax->variable_declaration->type->variable_type->name);
                
                Symbol * struct_symbol = get_symbol(symbol_table, struct_name);
                if(struct_symbol == NULL)
                {
                    char buf[100];
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
                char buf[100];
                sprintf(buf, "Use of undeclared identifier '%s'", syntax->variable->name);
                print_error(buf, syntax->variable->name, syntax->lineno);
                is_correct = false;
            }
            else if(previous_symbol->declaration->type != VARIABLE_DECLARATION || previous_symbol->declaration->variable_declaration->type->variable_type->type == STRUCT)
            {
                char buf[100];
                if(previous_symbol->declaration->type == ARRAY_DECLARATION)
                    sprintf(buf, "Require '[]' on array variable");
                else if(previous_symbol->declaration->type == VARIABLE_DECLARATION && 
                   previous_symbol->declaration->variable_declaration->type->variable_type->type == STRUCT)
                    sprintf(buf, "Require '.' on array variable");
                else if(previous_symbol->declaration->type == FUNCTION_DECLARATION)
                    sprintf(buf, "Require '()' on function call");
            }
            break;
        }
        case ARRAY_DECLARATION:
        {
            Symbol * previous_symbol = get_symbol(symbol_table, syntax->array_declaration->name);

            if(previous_symbol == NULL)
            {
                Symbol *symbol = symbol_new();
                symbol->level = cur_level;
                strcpy(symbol->name, syntax->array_declaration->name);
                symbol->declaration = syntax;
                insert_symbol(symbol_table, symbol);
            }
            // redefinition check
            else if(previous_symbol->level == cur_level)
            {
                char buf[100];
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
                char buf[100];
                sprintf(buf, "Use of undeclared identifier '%s'", syntax->array_variable->name);
                print_error(buf, syntax->array_variable->name, syntax->lineno);
                is_correct = false;
            }
            // '[]' used on non-array variable
            else if(previous_symbol->declaration->type != ARRAY_DECLARATION)
            {
                char buf[100];
                sprintf(buf, "'[]' used on non-array variable '%s'", syntax->array_variable->name);
                print_error(buf, syntax->array_variable->name, syntax->lineno);
                is_correct = false;
            }
            break;
        }
        case STRUCT_DECLARATION:
        {
            char struct_name[100];
            sprintf(struct_name, "struct %s", syntax->struct_declaration->name);

            Symbol * previous_symbol = get_symbol(symbol_table, struct_name);
            if(previous_symbol == NULL)
            {
                Symbol *symbol = symbol_new();
                symbol->level = cur_level;
                strcpy(symbol->name, struct_name);
                symbol->declaration = syntax;
                insert_symbol(symbol_table, symbol);
            }
            // redefinition check
            else if(previous_symbol->level == cur_level)
            {
                char buf[100];
                sprintf(buf, "Redefinition of '%s'", struct_name);
                print_error(buf, struct_name, syntax->lineno);
                print_note("Previous definition is here", syntax->struct_declaration->name, previous_symbol->declaration->lineno);
                is_correct = false;
            }
            ENTER_SCOPE();
            if(!semantic_analysis(syntax->struct_declaration->block))
                is_correct = false;
            LEAVE_SCOPE();
            break;
        }
        case STRUCT_VARIABLE:
        {
            Symbol * previous_symbol = get_symbol(symbol_table, syntax->struct_variable->name);
            
            // undefined variable check
            if(previous_symbol == NULL)
            {
                char buf[100];
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
                    char struct_name[100];
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
                        char buf[100];
                        sprintf(buf, "Illegal member access of member '%s'", syntax->struct_variable->member);
                        print_error(buf, syntax->struct_variable->member, syntax->lineno);
                        is_correct = false;
                    }
                }
                // '.' used on non-struct variable
                else
                {
                    char buf[100];
                    sprintf(buf, "'.' used on non-struct variable '%s'", syntax->struct_variable->name);
                    print_error(buf, syntax->struct_variable->name, syntax->lineno);
                    is_correct = false;
                }
            }
            break;
        }
        case IF_STATEMENT:
        {
            // condition type
            if(check_expression_type(syntax->if_statement->condition) == NULL)
                is_correct = false;

            ENTER_SCOPE();
            if(!semantic_analysis(syntax->if_statement->then_body))
                is_correct = false;
            LEAVE_SCOPE();
            ENTER_SCOPE();
            if(!semantic_analysis(syntax->if_statement->else_body))
                is_correct = false;
            LEAVE_SCOPE();
            break;
        }
        case RETURN_STATEMENT:
        {
            // return type doesn't match the function type
            Syntax * type = check_expression_type(syntax->return_statement->expression);
            if(!is_variable_type_equal(cur_function->function_declaration->type, type))
            {
                char buf[100];
                char return_type[100];
                char function_type[100];
                variable_type_to_string(type, return_type);
                variable_type_to_string(cur_function->function_declaration->type, function_type);
                sprintf(buf, "Return type '%s'' doesn't match the function type '%s'", return_type, function_type);
                print_error(buf, "return", syntax->lineno);
                is_correct = false;
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
            else if(previous_symbol->level == cur_level)
            {
                char buf[100];
                sprintf(buf, "Redefinition of '%s'", syntax->function_declaration->name);
                print_error(buf, syntax->function_declaration->name, syntax->lineno);
                print_note("Previous definition is here", syntax->function_declaration->name, previous_symbol->declaration->lineno);
                is_correct = false;
            }
            ENTER_SCOPE();
            old_function = cur_function;
            cur_function = syntax;

            if(!semantic_analysis(syntax->function_declaration->arguments))
                is_correct = false;
            if(!semantic_analysis(syntax->function_declaration->block))
                is_correct = false;
            LEAVE_SCOPE();
            cur_function = old_function;
            break;
        }
        case FUNCTION_CALL:
        {
            Symbol * previous_symbol = get_symbol(symbol_table, syntax->function_call->name);

            // undefined function call
            if(previous_symbol == NULL)
            {
                char buf[100];
                sprintf(buf, "Undefined function name '%s'", syntax->function_call->name);
                print_error(buf, syntax->function_call->name, syntax->lineno);
                is_correct = false;
            }
            // '()' used on non-function
            else if(previous_symbol->declaration->type != FUNCTION_DECLARATION)
            {
                char buf[100];
                sprintf(buf, "'()' used on non-function '%s'", syntax->function_call->name);
                print_error(buf, syntax->function_call->name, syntax->lineno);
                is_correct = false;
            }
            else
            {
                // arguments don't match
                int declared_number = 0;
                int call_number = 0;

                if(previous_symbol->declaration->function_declaration->arguments != NULL)
                    declared_number = list_length(previous_symbol->declaration->function_declaration->arguments->block->statements);
                if(syntax->function_call->arguments != NULL)
                    call_number = list_length(syntax->function_call->arguments->block->statements);
                    
                if(declared_number != call_number)
                {
                    char buf[100];
                    sprintf(buf, "Function arguments number doesn't match, expected %d, has %d'", declared_number, call_number);
                    print_error(buf, syntax->function_call->name, syntax->lineno);
                    is_correct = false;
                }
                else if(declared_number != 0)
                {
                    for(int i = 0; i < list_length(previous_symbol->declaration->function_declaration->arguments->block->statements); ++i)
                    {
                        Syntax * call_type = check_expression_type((Syntax *)list_get(syntax->function_call->arguments->block->statements, i));
                        Syntax * declaration = (Syntax *)list_get(previous_symbol->declaration->function_declaration->arguments->block->statements, i);
                        Syntax * declared_type = NULL;
                        if(declaration->type == VARIABLE_DECLARATION)
                            declared_type = declaration->variable_declaration->type;
                        else if(declaration->type == ARRAY_DECLARATION)
                            declared_type = declaration->array_declaration->type;

                        if(call_type == NULL)
                            is_correct = false;
                        else
                        {
                            if(!is_variable_type_equal(call_type, declared_type))
                            {
                                char buf[100];
                                char call_type_str[100];
                                char declared_type_str[100];
                                variable_type_to_string(call_type, call_type_str);
                                variable_type_to_string(declared_type, declared_type_str);
                                sprintf(buf, "Function %d argument type doesn't match, expected '%s', has '%s'", i + 1, declared_type_str, call_type_str);
                                print_error(buf, syntax->function_call->name, syntax->lineno);
                                is_correct = false;
                            }
                        }
                    }
                }
            }
            break;
        }
        case ASSIGNMENT:
        {
            // inconsistent type
            Syntax * expression_type = check_expression_type(syntax->assignment->expression);
            Syntax * dest_type = check_expression_type(syntax->assignment->dest);
            if(expression_type == NULL || dest_type == NULL)
                break;
            if(!is_variable_type_equal(expression_type, dest_type))
            {
                char buf[100];
                char dest_type_str[100];
                char expression_type_str[100];
                variable_type_to_string(dest_type, dest_type_str);
                variable_type_to_string(expression_type, expression_type_str);
                sprintf(buf, "Assignment has inconsistent types, expected '%s', has '%s'", dest_type_str, expression_type_str);
                print_error(buf, "=", syntax->lineno);
                is_correct = false;
            }
            break;
        }
        case WHILE_STATEMENT:
        {
            if(check_expression_type(syntax->while_statement->condition) == NULL)
                is_correct = false;

            // condition type check
            ENTER_SCOPE();
            if(!semantic_analysis(syntax->while_statement->body))
                is_correct = false;
            LEAVE_SCOPE();
            break;
        }
        case BLOCK:
        {
            for(int i = 0; i < list_length(syntax->block->statements); ++i)
            {
                if(!semantic_analysis((Syntax *)list_get(syntax->block->statements, i)))
                    is_correct = false;
            }
            break;
        }
        case TOP_LEVEL:
        {
            // initialize the symbol table
            symbol_table = new_symbol_table();  
            cur_function = syntax;

            for(int i = 0; i < list_length(syntax->top_level->statements); ++i)
            {
                if(!semantic_analysis((Syntax *)list_get(syntax->top_level->statements, i)))
                    is_correct = false;
            }
                
            break;
        }
        default: printf("Error!Undefined type %d!\n", syntax->type); break;
    }
    return is_correct;
}