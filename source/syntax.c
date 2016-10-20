#include <stdlib.h>
#include "syntax.h"

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
            syntax->immediate->value = 0;
            break;
        }
        case VARIABLE:
        {
            syntax->variable = (Variable *)malloc(sizeof(Variable));
            syntax->variable->type = INT;
            syntax->variable->name = (char *)malloc(sizeof(char) * 33);
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
            syntax->if_statement->then = NULL;
            break;
        }
        case RETURN_STATEMENT:
        {
            syntax->return_statement = (ReturnStatement *)malloc(sizeof(ReturnStatement));
            syntax->return_statement->expression = NULL;
            break;
        }
        case DEFINE_VARIABLE_STATEMENT:
        {
            syntax->define_var_statement = (DefineVarStatement *)malloc(sizeof(DefineVarStatement));
            syntax->define_var_statement->name = string_new(33);
            syntax->define_var_statement->init_value = NULL;
            break;
        }
        case FUNCTION:
        {
            syntax->function = (Function *)malloc(sizeof(Function));
            syntax->function->name = string_new(33);
            syntax->function->parameters = NULL;
            syntax->function->root_block = NULL;
            break;
        }
        case FUNCTION_CALL:
        {
            syntax->function_call = (FunctionCall *)malloc(sizeof(FunctionCall));
            syntax->function_call->name = string_new(33);
            syntax->function_call->arguments = NULL;
            break;
        }
        case FUNCTION_ARGUMENTS:
        {
            syntax->function_arguments = (FunctionArguments *)malloc(sizeof(FunctionArguments));
            syntax->function_arguments->arguments = NULL;
            break;
        }
        case ASSIGNMENT:
        {
            syntax->assignment = (Assignment *)malloc(sizeof(Assignment));
            syntax->assignment->name = string_new(33);
            syntax->assignment->expression = NULL;
            break;
        }
        case WHILE_SYNTAX:
        {
            syntax->while_statement = (WhileStatement *)malloc(sizeof(WhileStatement));
            syntax->while_statement->condition = NULL;
            syntax->while_statement->body = NULL;
            break;
        }
        case TOP_LEVEL:
        {
            syntax->top_level = (TopLevel *)malloc(sizeof(TopLevel));
            syntax->top_level->definitions = NULL;
            break;
        }
        default: break;
    }
    return syntax;
}

void default_syntax_delete(Syntax * syntax)
{

}

Syntax * syntax_new(SyntaxType type)
{
    return default_syntax_new(type);
}

void syntax_delete(Syntax * syntax)
{
    default_syntax_delete(syntax);
    return;
}



/*
Syntax * create_string_node(int type, char string_value[128])
{
    Syntax * node = (TreeNode *)malloc(sizeof(TreeNode));
    node->type = type;
    strcpy(node->value.string_value, string_value);
    node->lineno = yylineno;
    node->next = NULL;
    node->upper_layer = NULL;
    node->lower_layer = NULL;
    return node;
}

Syntax * create_int_node(int type, int int_value)
{
    Syntax * node = (TreeNode *)malloc(sizeof(TreeNode));
    node->type = type;
    node->value.int_value = int_value;
    node->lineno = yylineno;
    node->next = NULL;
    node->upper_layer = NULL;
    node->lower_layer = NULL;
    return node;
}


void traverse(Syntax * node, int depth)
{
    if(node == NULL)
        return;
    
    Syntax * temp = node;
    while(temp != NULL)
    {
        print_node(temp, depth);
        traverse(temp->lower_layer, depth + 2);
        temp = temp->next;
    }
    
}

void print_node(Syntax * node, int depth)
{
    for(int i = 0;i < depth; i++)
        putchar(' ');
    
    switch(node->type)
    {
    case INTEGER:
        printf("%d \n", node->value.int_value);
        break;
    default:
        printf("%s \n", node->value.string_value);
    }    
}

Syntax * reduce(char name[128], int count, ...)
{
    Syntax * upper_layer = create_string_node(NON_TERMINAL, name);
    va_list argp;
    va_start(argp, count);
    Syntax * head = NULL;
    Syntax * temp = NULL;
    
    Syntax * cur = NULL;
    for(int i = 0; i < count;i ++)
    {
        cur = va_arg(argp, Syntax *);
        if(cur == NULL)
            continue;
            
        if(temp == NULL)
        {
            temp = cur;
            head = cur;
            head->upper_layer = upper_layer;
        }
        else
        {
            temp->next = cur;
            temp = temp->next;
            temp->upper_layer = upper_layer;
        }
    }
    upper_layer->lower_layer = head;
    va_end(argp);
    return upper_layer;
}*/