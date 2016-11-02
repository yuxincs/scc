#include "semantic.h"

void semantic_analysis(Syntax * syntax)
{
    if(syntax == NULL)
    {
        printf("(null)\n");
        return;
    }

    switch(syntax->type)
    {
        case IMMEDIATE:
        {
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
            printf("VariableType : %d %s\n", (int)syntax->variable_type->type, syntax->variable_type->name);
            break;
        }
        case VARIABLE_DECLARATION:
        {
            // insert into symbol table
            printf("VariableDeclaration : %s\n", syntax->variable_declaration->name);
            semantic_analysis(syntax->variable_declaration->type);
            break;
        }
        case VARIABLE:
        {
            // check the symbol table
            printf("Variable : %s\n", syntax->variable->name);
            break;
        }
        case ARRAY_DECLARATION:
        {
            // insert into symbol table
            printf("ArrayDeclaration : %s Length: %d\n", syntax->array_declaration->name, syntax->array_declaration->length);
            semantic_analysis(syntax->array_declaration->type);
            break;
        }
        case ARRAY_VARIABLE:
        {
            // check the symbol table
            printf("Array : %s index: %d\n", syntax->array_variable->name, syntax->array_variable->index);
            break;
        }
        case STRUCT_DECLARATION:
        {
            printf("StructDeclaration : %s\n", syntax->struct_declaration->name);
            semantic_analysis(syntax->struct_declaration->block);
            break;
        }
        case UNARY_EXPRESSION:
        {
            printf("UnaryExpression : type %d\n", (int) syntax->unary_expression->type);
            semantic_analysis(syntax->unary_expression->expression);
            break;
        }
        case BINARY_EXPRESSION:
        {
            printf("BinaryExpression : type %d\n", (int) syntax->binary_expression->type);
            printf("Left:\n");
            semantic_analysis(syntax->binary_expression->left);
            printf("Right:\n");
            semantic_analysis(syntax->binary_expression->right);
            break;
        }
        case IF_STATEMENT:
        {
            printf("IfStatement\n");
            printf("Condition:\n");
            semantic_analysis(syntax->if_statement->condition);
    
            printf("Then:\n");
            semantic_analysis(syntax->if_statement->body);
            break;
        }
        case RETURN_STATEMENT:
        {
            printf("ReturnStatement\n");
            semantic_analysis(syntax->return_statement->expression);
            break;
        }
        case FUNCTION_DECLARATION:
        {
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
            printf("FunctionCall : %s\n", syntax->function_call->name);
           
            printf("Arguments: \n");
            semantic_analysis(syntax->function_call->arguments);
            break;
        }
        case ASSIGNMENT:
        {
            printf("Assignment :\n");
            semantic_analysis(syntax->assignment->dest);
            semantic_analysis(syntax->assignment->expression);
            break;
        }
        case WHILE_STATEMENT:
        {
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
            
            printf("TopLevel\n");
            for(int i = 0; i < list_length(syntax->top_level->statements); ++i)
                semantic_analysis((Syntax *)list_get(syntax->top_level->statements, i));
            
            break;
        }
        default: printf("Error!Undefined type!\n"); break;
    }
    return;
}