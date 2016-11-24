%{ 
    #include <stdio.h>
    #include <stdlib.h>
    #include <stdbool.h>
    #include <string.h>
    #include <stdarg.h>
    #include "commonutils.h"
    #include "list.h"
    #include "syntax.h"

    // lex variable declarations
    extern int yylex();
    extern FILE * yyin;
    extern int yylineno;
    extern char * yytext;

    // top level syntax definition exists in main.c
    static Syntax * top_level = NULL;
    
    int yyerror(char * msg);
%}
%union
{
    int int_value;
    float float_value;
    char * string_value;
    Syntax * syntax_value;
}
%token L_WHILE L_DO L_IF L_INCLUDE L_RETURN L_ELSE L_STRUCT L_VOID
%token L_LESS_OR_EQUAL L_GREATER_OR_EQUAL L_AND L_OR L_EQUAL L_NOT_EQUAL
%token <int_value> L_INTEGER
%token <float_value> L_FLOAT
%token <string_value> L_IDENTIFIER L_BASIC_TYPE
%token '+' '-' '*' '/' '#' '<' '>' '=' '(' ')' ',' ';' '.' '~' '!'
%type <syntax_value> program statement_list statement variable_declaration_statement
%type <syntax_value> variable_declaration_list variable_declaration function_declaration_statement
%type <syntax_value> argument_list argument struct_declaration_statement
%type <syntax_value> in_block_statement_list expression basic_type struct_type void_type variable_declaration_statement_list
%type <syntax_value> while_statement if_statement in_block_statement expression_list variable
%left '='
%left '<'
%left '+'
%left '-'
%left '*'
%nonassoc '!'
%nonassoc '~'

%%
program:
        statement_list
        {
            Syntax * cur_syntax = $1;
            if(cur_syntax != NULL)
            {
                top_level = syntax_new(TOP_LEVEL);
                top_level->top_level->statements = cur_syntax->block->statements;
                syntax_delete(cur_syntax);
            }   
        }
        ;

statement_list:
        statement statement_list
        {
            Syntax * statement_list = $2;

            // if it is the last one of the list
            if(statement_list == NULL)
            {
                statement_list = syntax_new(BLOCK);
                statement_list->block->statements = list_new();
            }

            list_prepend(statement_list->block->statements, (void *)$1);
            $$ = statement_list;
        }
        |
        {
            $$ = NULL;
        }
        ;

statement:
        variable_declaration_statement
        {
            $$ = $1;
        }
        |
        struct_declaration_statement
        {
            $$ = $1;
        }
        |
        function_declaration_statement
        {
            $$ = $1;
        }
        ;

basic_type:
        L_BASIC_TYPE
        {
            Syntax * syntax = syntax_new(VARIABLE_TYPE);
            if(strcmp($1, "int") == 0)
                syntax->variable_type->type = INT;
            else if(strcmp($1, "float") == 0)
                syntax->variable_type->type = FLOAT;
            $$ = syntax;
        }
        ;

struct_type:
        L_STRUCT L_IDENTIFIER
        {
            Syntax * syntax = syntax_new(VARIABLE_TYPE);
            syntax->variable_type->type = STRUCT;
            strcpy(syntax->variable_type->name, $2);
            $$ = syntax;
        }
        ;

void_type:
        L_VOID
        {
            Syntax * syntax = syntax_new(VARIABLE_TYPE);
            syntax->variable_type->type = VOID;
            $$ = syntax;
        }
        ;

struct_declaration_statement:
        L_STRUCT L_IDENTIFIER '{' variable_declaration_statement_list '}' ';'
        {
            Syntax * syntax = syntax_new(STRUCT_DECLARATION);
            strcpy(syntax->struct_declaration->name, $2);
            syntax->struct_declaration->block = $4;
            $$ = syntax;
        }
        ;

variable_declaration_statement_list:
        variable_declaration_statement variable_declaration_statement_list
        {
            Syntax * statement = $1;
            Syntax * statement_list = $2;
            list_append_list(statement->block->statements, statement_list->block->statements);
            list_delete(statement_list->block->statements);
            syntax_delete(statement_list);
            $$ = statement;
        }
        |
        variable_declaration_statement
        {
            $$ = $1;
        }
        ;


variable_declaration_statement:
        basic_type variable_declaration_list ';'
        {
            Syntax * list = $2;
            
            // iterate through the list and set the declaration type
            for(int i = 0;i < list_length(list->block->statements); ++i)
            {
                Syntax * syntax = list_get(list->block->statements, i);
                if(syntax->type == VARIABLE_DECLARATION)
                    syntax->variable_declaration->type = $1;
                else if(syntax->type == ARRAY_DECLARATION)
                    syntax->array_declaration->type = $1;
            }
            $$ = list;
        }
        |
        struct_type variable_declaration_list ';'
        {
            Syntax * list = $2;
            
            // iterate through the list and set the declaration type
            for(int i = 0;i < list_length(list->block->statements); ++i)
            {
                Syntax * syntax = list_get(list->block->statements, i);
                if(syntax->type == VARIABLE_DECLARATION)
                    syntax->variable_declaration->type = $1;
                else if(syntax->type == ARRAY_DECLARATION)
                    syntax->array_declaration->type = $1;
            }
            $$ = list;
        }
        ;

variable_declaration_list:
        variable_declaration ',' variable_declaration_list
        {
            Syntax * variable = $1;
            Syntax * syntax = $3;
            if(variable->type == BLOCK)
            {
                list_append_list(variable->block->statements, syntax->block->statements);
                list_delete(syntax->block->statements);
                syntax->block->statements = variable->block->statements;
                syntax_delete(variable);
            }
            else
            {
                list_prepend(syntax->block->statements, (void *) variable);
            }
            
            $$ = syntax;
        }
        |
        variable_declaration
        {
            Syntax * syntax = $1;
            
            if(syntax->type != BLOCK)
            {
                syntax = syntax_new(BLOCK);
                syntax->block->statements = list_new();
                list_prepend(syntax->block->statements, (void *)$1);
            }
            $$ = syntax;
        }
        ;

variable_declaration:
        variable '=' expression
        {
            Syntax * block = syntax_new(BLOCK);
            block->block->statements = list_new();


            Syntax * variable = $1;
            Syntax * declaration = NULL;
            if(variable->type == VARIABLE)
            {
                declaration = syntax_new(VARIABLE_DECLARATION);
                strcpy(declaration->variable_declaration->name, variable->variable->name);
            }
            else if (variable->type == ARRAY_VARIABLE)
            {
                declaration = syntax_new(ARRAY_DECLARATION);
                strcpy(declaration->array_declaration->name, variable->array_variable->name);
                declaration->array_declaration->length = variable->array_variable->index;
            }
            else
            {
                printf("Illegal variable type!\n");
            }

            Syntax * assign = syntax_new(ASSIGNMENT);
            assign->assignment->dest = variable;
            assign->assignment->expression = $3;
            list_append(block->block->statements, declaration);
            list_append(block->block->statements, assign);
            $$ = block;
        }
        |
        variable
        {
            // Convert variable syntax into variable declaration syntax
            Syntax * variable = $1;
            Syntax * declaration = NULL;
            if(variable->type == VARIABLE)
            {
                declaration = syntax_new(VARIABLE_DECLARATION);
                    strcpy(declaration->variable_declaration->name, variable->variable->name);
            }
            else if (variable->type == ARRAY_VARIABLE)
            {
                declaration = syntax_new(ARRAY_DECLARATION);
                    strcpy(declaration->array_declaration->name, variable->array_variable->name);
                    declaration->array_declaration->length = variable->array_variable->index;
            }
            else
            {
                printf("Illegal variable type!\n");
            }
            syntax_delete(variable);
            $$ = declaration;
        }
        ;

variable:
        L_IDENTIFIER
        {
            Syntax * syntax = syntax_new(VARIABLE);
            strcpy(syntax->variable->name, $1);
            $$ = syntax;
        }
        |
        L_IDENTIFIER '[' L_INTEGER ']'
        {
            Syntax * syntax = syntax_new(ARRAY_VARIABLE);
            strcpy(syntax->array_variable->name, $1);
            syntax->array_variable->index = $3;
            $$ = syntax;
        }
        |
        L_IDENTIFIER '.' L_IDENTIFIER
        {
            Syntax * syntax = syntax_new(STRUCT_VARIABLE);
            strcpy(syntax->struct_variable->name, $1);
            strcpy(syntax->struct_variable->member, $3);
            $$ = syntax;
        }
        ;


function_declaration_statement:
        basic_type L_IDENTIFIER '(' ')' '{' in_block_statement_list '}'
        {
            Syntax * function = syntax_new(FUNCTION_DECLARATION);
            function->function_declaration->type = $1;
            strcpy(function->function_declaration->name, $2);
            function->function_declaration->block = $6;
            $$ = function;
        }
        |
        basic_type L_IDENTIFIER '(' argument_list ')' '{' in_block_statement_list '}'
        {
            Syntax * function = syntax_new(FUNCTION_DECLARATION);
            function->function_declaration->type = $1;
            strcpy(function->function_declaration->name, $2);
            function->function_declaration->arguments = $4;
            function->function_declaration->block = $7;
            $$ = function;
        }
        |
        void_type L_IDENTIFIER '(' ')' '{' in_block_statement_list '}'
        {
            Syntax * function = syntax_new(FUNCTION_DECLARATION);
            function->function_declaration->type = $1;
            strcpy(function->function_declaration->name, $2);
            function->function_declaration->block = $6;
            $$ = function;
        }
        |
        void_type L_IDENTIFIER '(' argument_list ')' '{' in_block_statement_list '}'
        {
            Syntax * function = syntax_new(FUNCTION_DECLARATION);
            function->function_declaration->type = $1;
            strcpy(function->function_declaration->name, $2);
            function->function_declaration->arguments = $4;
            function->function_declaration->block = $7;
            $$ = function;
        }
        ;

argument_list:
        argument ',' argument_list
        {
            Syntax * syntax = $3;
            list_prepend(syntax->block->statements, (void *)$1);
            $$ = syntax;
        }
        |
        argument
        {
            Syntax * syntax = syntax_new(BLOCK);
            syntax->block->statements = list_new();
            list_prepend(syntax->block->statements, (void *)$1);
            $$ = syntax;
        }
        ;

argument:
        basic_type L_IDENTIFIER
        {
            Syntax * syntax = syntax_new(VARIABLE_DECLARATION);
            syntax->variable_declaration->type = $1;
            strcpy(syntax->variable_declaration->name, $2);
            $$ = syntax;
        }
        |
        struct_type L_IDENTIFIER
        {
            Syntax * syntax = syntax_new(VARIABLE_DECLARATION);
            syntax->variable_declaration->type = $1;
            strcpy(syntax->variable_declaration->name, $2);
            $$ = syntax;
        }
        ;

expression_list:
        expression ',' expression_list
        {
            Syntax * list = $3;
            list_prepend(list->block->statements, (void *)$1);
            $$ = list;
        }
        |
        expression
        {
            Syntax * list = syntax_new(BLOCK);
            list->block->statements = list_new();
            list_prepend(list->block->statements, (void *)$1);
            $$ = list;
        }
        ;

expression:
        L_INTEGER
        {
            Syntax * syntax = syntax_new(IMMEDIATE);
            syntax->immediate->type = INT;
            syntax->immediate->int_value = $1;
            $$ = syntax;
        }
        |
        L_FLOAT
        {
            Syntax * syntax = syntax_new(IMMEDIATE);
            syntax->immediate->type = FLOAT;
            syntax->immediate->float_value = $1;
            $$ = syntax;
        }
        |
        variable
        {
            $$ = $1;
        }
        |
        L_IDENTIFIER '(' expression_list ')'
        {
            Syntax * syntax = syntax_new(FUNCTION_CALL);
            strcpy(syntax->function_call->name, $1);
            syntax->function_call->arguments = $3;
            $$ = syntax;
        }
        |
        L_IDENTIFIER '(' ')'
        {
            Syntax * syntax = syntax_new(FUNCTION_CALL);
            strcpy(syntax->function_call->name, $1);
            $$ = syntax;
        }
        |
        expression '+' expression
        {
            Syntax * syntax = syntax_new(BINARY_EXPRESSION);
            syntax->binary_expression->type = ADDITION;
            syntax->binary_expression->left = $1;
            syntax->binary_expression->right = $3;
            $$ = syntax;
        }
        |
        expression '-' expression
        {
            Syntax * syntax = syntax_new(BINARY_EXPRESSION);
            syntax->binary_expression->type = SUBTRACTION;
            syntax->binary_expression->left = $1;
            syntax->binary_expression->right = $3;
            $$ = syntax;
        }
        |
        expression '*' expression
        {
            Syntax * syntax = syntax_new(BINARY_EXPRESSION);
            syntax->binary_expression->type = MULTIPLICATION;
            syntax->binary_expression->left = $1;
            syntax->binary_expression->right = $3;
            $$ = syntax;
        }
        |
        expression '/' expression
        {
            Syntax * syntax = syntax_new(BINARY_EXPRESSION);
            syntax->binary_expression->type = DIVISION;
            syntax->binary_expression->left = $1;
            syntax->binary_expression->right = $3;
            $$ = syntax;
        }
        |
        '!' expression
        {
            Syntax * syntax = syntax_new(UNARY_EXPRESSION);
            syntax->unary_expression->type = LOGICAL_NEGATION;
            syntax->unary_expression->expression = $2;
            $$ = syntax;
        }
        |
        '~' expression
        {
            Syntax * syntax = syntax_new(UNARY_EXPRESSION);
            syntax->unary_expression->type = BITWISE_NEGATION;
            syntax->unary_expression->expression = $2;
            $$ = syntax;
        }
        |
        expression L_EQUAL expression
        {
            Syntax * syntax = syntax_new(BINARY_EXPRESSION);
            syntax->binary_expression->type = EQUAL;
            syntax->binary_expression->left = $1;
            syntax->binary_expression->right = $3;
            $$ = syntax;
        }
        |
        expression L_NOT_EQUAL expression
        {
            Syntax * syntax = syntax_new(BINARY_EXPRESSION);
            syntax->binary_expression->type = NOT_EQUAL;
            syntax->binary_expression->left = $1;
            syntax->binary_expression->right = $3;
            $$ = syntax;
        }
        |
        expression L_GREATER_OR_EQUAL expression
        {
            Syntax * syntax = syntax_new(BINARY_EXPRESSION);
            syntax->binary_expression->type = GREATER_OR_EQUAL;
            syntax->binary_expression->left = $1;
            syntax->binary_expression->right = $3;
            $$ = syntax;
        }
        |
        expression '>' expression
        {
            Syntax * syntax = syntax_new(BINARY_EXPRESSION);
            syntax->binary_expression->type = GREATER;
            syntax->binary_expression->left = $1;
            syntax->binary_expression->right = $3;
            $$ = syntax;
        }
        |
        expression '<' expression
        {
            Syntax * syntax = syntax_new(BINARY_EXPRESSION);
            syntax->binary_expression->type = LESS;
            syntax->binary_expression->left = $1;
            syntax->binary_expression->right = $3;
            $$ = syntax;
        }
        |
        expression L_LESS_OR_EQUAL expression
        {
            Syntax * syntax = syntax_new(BINARY_EXPRESSION);
            syntax->binary_expression->type = LESS;
            syntax->binary_expression->left = $1;
            syntax->binary_expression->right = $3;
            $$ = syntax;
        }
        |
        '(' expression ')'
        {
            $$ = $2;
        }
        ;

in_block_statement_list:
        in_block_statement in_block_statement_list
        {
            Syntax * syntax = $2;
            if(syntax == NULL)
            {
                syntax = syntax_new(BLOCK);
                syntax->block->statements = list_new();
            }

            list_prepend(syntax->block->statements, $1);
            $$ = syntax;
        }
        |
        {
            $$ = NULL;
        }
        ;

in_block_statement:
        if_statement
        {
            $$ = $1;
        }
        |
        while_statement
        {
            $$ = $1;
        }
        |
        variable_declaration_statement
        {
            $$ = $1;
        }
        |
        variable '=' expression ';'
        {
            Syntax * syntax = syntax_new(ASSIGNMENT);
            syntax->assignment->dest = $1;
            syntax->assignment->expression = $3;
            $$ = syntax;
        }
        |
        L_IDENTIFIER '(' expression_list ')' ';'
        {
            Syntax * syntax = syntax_new(FUNCTION_CALL);
            strcpy(syntax->function_call->name, $1);
            syntax->function_call->arguments = $3;
            $$ = syntax;
        }
        |
        L_IDENTIFIER '(' ')' ';'
        {
            Syntax * syntax = syntax_new(FUNCTION_CALL);
            strcpy(syntax->function_call->name, $1);
            $$ = syntax;
        }
        |
        L_RETURN expression ';'
        {
            Syntax * syntax = syntax_new(RETURN_STATEMENT);
            syntax->return_statement->expression = $2;
            $$ = syntax;
        }
        |
        L_RETURN ';'
        {
            Syntax * syntax = syntax_new(RETURN_STATEMENT);
            $$ = syntax;
        }
        ;

if_statement:
    L_IF '(' expression ')' '{' in_block_statement_list '}'
    {
        Syntax * syntax = syntax_new(IF_STATEMENT);
        syntax->if_statement->condition = $3;
        syntax->if_statement->then_body = $6;
        $$ = syntax;
    }
    |
    L_IF '(' expression ')' in_block_statement
    {
        Syntax * syntax = syntax_new(IF_STATEMENT);
        syntax->if_statement->condition = $3;
        syntax->if_statement->then_body = $5;
        $$ = syntax;
    }
    |
    L_IF '(' expression ')' '{' in_block_statement_list '}' L_ELSE '{' in_block_statement_list '}'
    {
        Syntax * syntax = syntax_new(IF_STATEMENT);
        syntax->if_statement->condition = $3;
        syntax->if_statement->then_body = $6;
        syntax->if_statement->else_body = $10;
        $$ = syntax;
    }
    |
    L_IF '(' expression ')' in_block_statement L_ELSE in_block_statement
    {
        Syntax * syntax = syntax_new(IF_STATEMENT);
        syntax->if_statement->condition = $3;
        syntax->if_statement->then_body = $5;
        syntax->if_statement->else_body = $7;
        $$ = syntax;
    }
    |
    L_IF '(' expression ')' '{' in_block_statement_list '}' L_ELSE in_block_statement
    {
        Syntax * syntax = syntax_new(IF_STATEMENT);
        syntax->if_statement->condition = $3;
        syntax->if_statement->then_body = $6;
        syntax->if_statement->else_body = $9;
        $$ = syntax;
    }
    |
    L_IF '(' expression ')' in_block_statement L_ELSE '{' in_block_statement_list '}'
    {
        Syntax * syntax = syntax_new(IF_STATEMENT);
        syntax->if_statement->condition = $3;
        syntax->if_statement->then_body = $5;
        syntax->if_statement->else_body = $8;
        $$ = syntax;
    }
    ;

while_statement:
    L_WHILE '(' expression ')' '{' in_block_statement_list '}'
    {
        Syntax * syntax = syntax_new(WHILE_STATEMENT);
        syntax->while_statement->condition = $3;
        syntax->while_statement->body = $6;
        $$ = syntax;
    }
    |
    L_WHILE '(' expression ')' in_block_statement
    {
        Syntax * syntax = syntax_new(WHILE_STATEMENT);
        syntax->while_statement->condition = $3;
        syntax->while_statement->body = $5;
        $$ = syntax;
    }
    ;


%%
Syntax * syntax_analysis(FILE * source_file)
{
    yyin = source_file;
    if(yyparse() != 0)
        return NULL;
    else
        return top_level;
}

int yyerror(char *msg)
{
    print_error(msg, yytext, yylineno);
    return 0;
}
