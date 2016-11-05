#include <assert.h>
#include "commonutils.h"
#include "semantic.h"

#define ENTER_SCOPE(x) { old_scope = cur_scope; cur_level++; cur_scope = x; }
#define LEAVE_SCOPE() { cur_scope = old_scope; remove_level(symbol_table, cur_level); cur_level--; }

SymbolTable * symbol_table = NULL;

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
                    
                    // if the variable's type is incomplete abandon the check
                    if(struct_symbol == NULL)
                        break;

                    Syntax * block = struct_symbol->declaration->struct_declaration->block;
                    bool has_member = false;
                    for(int i = 0; i < list_length(block->block->statements); ++i)
                    {
                        Syntax * member = list_get(block->block->statements, i);
                        if((member->type == VARIABLE_DECLARATION && strcmp(member->variable_declaration->name, syntax->struct_variable->name) == 0) ||
                           (member->type == ARRAY_DECLARATION && strcmp(member->array_declaration->name, syntax->struct_variable->name) == 0))
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
            printf("UnaryExpression : type %d\n", (int) syntax->unary_expression->type);
            semantic_analysis(syntax->unary_expression->expression);
            break;
        }
        case BINARY_EXPRESSION:
        {
            // type conflicts
            printf("BinaryExpression : type %d\n", (int) syntax->binary_expression->type);
            printf("Left:\n");
            semantic_analysis(syntax->binary_expression->left);
            printf("Right:\n");
            semantic_analysis(syntax->binary_expression->right);
            break;
        }
        case IF_STATEMENT:
        {
            // condition type
            printf("IfStatement\n");
            printf("Condition:\n");
            semantic_analysis(syntax->if_statement->condition);
    
            printf("Then:\n");
            semantic_analysis(syntax->if_statement->body);
            break;
        }
        case RETURN_STATEMENT:
        {
            // return type doesn't match the function type
            printf("ReturnStatement\n");
            semantic_analysis(syntax->return_statement->expression);
            break;
        }
        case FUNCTION_DECLARATION:
        {
            // duplicate declaration
            printf("FunctionDeclaration : %s\n", syntax->function_declaration->name);
           
            printf("ReturnType: \n");
            semantic_analysis(syntax->function_declaration->type);
            
            printf("Arguments: \n");
            semantic_analysis(syntax->function_declaration->arguments);
           
            printf("Body: \n");
            semantic_analysis(syntax->function_declaration->block);
            break;
        }
        case FUNCTION_CALL:
        {
            // undefined function call

            // '()' used on non-function

            // arguments don't match

            
            printf("FunctionCall : %s\n", syntax->function_call->name);
           
            printf("Arguments: \n");
            semantic_analysis(syntax->function_call->arguments);
            break;
        }
        case ASSIGNMENT:
        {
            // inconsistent type
            printf("Assignment :\n");
            semantic_analysis(syntax->assignment->dest);
            semantic_analysis(syntax->assignment->expression);
            break;
        }
        case WHILE_STATEMENT:
        {
            // condition type
            printf("WhileStatement\n");
            printf("Condition:\n");
            semantic_analysis(syntax->while_statement->condition);
           
            printf("Body:\n");
            semantic_analysis(syntax->while_statement->body);
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