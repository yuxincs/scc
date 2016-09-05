#include "syntax.h"

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
}