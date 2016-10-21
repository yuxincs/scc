%{ 
    #include <stdio.h>
    #include <stdlib.h>
    #include <stdbool.h>
    #include <string.h>
    #include <stdarg.h>
    #include "list.h"
    #include "syntax.h"

    // lex variable declarations
    extern int yylex();
    extern FILE * yyin;
    extern int yylineno;
    extern char * yytext;
    //extern int yydebug;

    // top level syntax definition exists in main.c
    extern Syntax * top_level;
    
    char file_name[1024];
    char *file_content[1024];
    int yyerror(char * msg);

    //yydebug = 1;
%}
%union
{
    int int_value;
    float float_value;
    char * string_value;
    Syntax * syntax_value;
}
%token CONST WHILE DO IF INCLUDE RETURN ELSE STRUCT VOID
%token LESS_OR_EQUAL GREATER_OR_EQUAL AND OR EQUAL NOT_EQUAL
%token <int_value> INTEGER
%token <float_value> FLOAT_NUMBER
%token <string_value> IDENTIFIER BASIC_TYPE
%token '+' '-' '*' '/' '#' '<' '>' '=' '(' ')' ',' ';' '.' '~' '!'
%type <syntax_value> program definition_list definition external_declaration_list
%type <syntax_value> specifier struct_specifier optional_tag variable_declaration
%type <syntax_value> function_declaration variable_list complex_statement
%type <syntax_value> statement_list statement definition_list declaration expression
%type <syntax_value> argument_list parameter_declaration 
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
                syntax_free(cur_syntax);
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
                syntax->block->statements = list_new();
            }

            list_prepend(syntax->block->statements, (void *)$1);
            $$ = syntax;
        }
        |
        {
            $$ = NULL;
        }
        ;

statement:
        variable_definition_statement
        {
            $$ = $1;
        }
        |
        struct_definition_statement
        {
            $$ = $1;
        }
        |
        function_definition_statement
        {
            $$ = $1;
        }
        ;

type:
        BASIC_TYPE
        {
            Syntax * syntax = syntax_new(VARIABLE_TYPE);
            syntax->variable_type->type = 
        }
        |
        STRUCT IDENTIFIER
        {

        }
        ;

variable_definition_statement:
        type variable_definition_list ';'
        {
            Syntax * syntax = $2;
            for(int i = 0;i < list_length(syntax->block->statements); ++i)
            {
                Syntax * variable_syntax = list_get(syntax->block->statements, i);
                variable_syntax->variable->type = $1;
            }
            $$ = syntax;
        }
        ;

variable_definition_list:
        variable_definition ',' variable_definition_list
        {
            Syntax * syntax = $3;
            list_prepend(syntax->block_statements, (void *)$1);
            $$ = syntax;
        }
        |
        variable_definition
        {
            Syntax * syntax = syntax_new(BLOCK);
            syntax->block->statements = list_new();
            list_prepend(syntax->block_statements, (void *)$1);
            $$ = syntax;
        }
        ;

variable_definition:
        IDENTIFIER
        {
            Syntax * syntax = syntax_new(VARIABLE);
            strcpy(syntax->variable->name, $1);
            $$ = syntax;
        }
        |
        IDENTIFIER '=' expression
        {
            Syntax * block = syntax_new(BLOCK);
            block->block->statements = list_new();
            Syntax * variable = syntax_new(VARIABLE);
            strcpy(syntax->variable->name, $1);
            Syntax * assign = syntax_new(ASSIGNMENT);
            strcpy(assign->assignment->name, $1);
            assign->expression = $3;
            list_append(block->block->statements, variable);
            list_append(block->block->statements, assign);
            $$ = syntax;
        }
        ;

function_definition_statement:
        function_type IDENTIFIER '(' ')' in_block_statement_list
        {
            Syntax * function = syntax_new(FUNCTION);
            function->function->type = $1;
            strcpy(function->function->name, $2);
            function->function->root_block = $6;
            $$ = function;
        }
        |
        function_type IDENTIFIER '(' argument_list ')' in_block_statement_list
        {
            Syntax * function = syntax_new(FUNCTION);
            function->function->type = $1;
            strcpy(function->function->name, $2);
            function->function->arguments = $4;
            function->function->root_block = $6;
            $$ = function;
        }
        ;

argument_list:
        argument ',' argument_list
        {
            Syntax * arguments = $3;
            list_prepend(arguments->function_arguments->arguments, (void *)$1);
            $$ = arguments;
        }
        |
        argument
        {
            Syntax * arguments = syntax_new(FUNCTION_ARGUMENTS);
            arguments->function_arguments->arguments = list_new();
            list_prepend(arguments->function_arguments->arguments, (void *)$1);
            $$ = arguments;
        }
        ;

argument:
        type 

external_declaration_list:
        variable_declaration
        {

        }
        |
        variable_declaration ',' external_declaration_list
        {

        }
        ;

specifier:
        TYPE
        {
            Syntax * syntax = syntax_new(VARIABLE_TYPE);
            if(strcmp($1, "int"))
                syntax->variable_type->type = INT;
            else if(strcmp($1, "float"))
                syntax->variable_type->type = FLOAT;
            $$ = syntax;
        }
        |
        struct_specifier
        {
            $$ = $1;
        }
        ;

struct_specifier:
        STRUCT optional_tag '{' definition_list '}'
        {
            Syntax * syntax = syntax_new(VARIABLE_TYPE);
            
        }
        |
        STRUCT IDENTIFIER
        {

        }
        ;

optional_tag:
        IDENTIFIER
        {

        }
        |
        ;



function_declaration:
        IDENTIFIER '(' variable_list ')'
        {
            Syntax * parameters = $3;
            Syntax * syntax = syntax_new(FUNCTION);
            strcpy(syntax->function->name, $1);
            syntax->function->parameters = parameters->block->statements;
            $$ = syntax;
        }
        |
        IDENTIFIER '(' ')'
        {
            Syntax * syntax = syntax_new(FUNCTION);
            strcpy(syntax->function->name, $1);
            $$ = syntax;
        }
        ;


%%
int yyerror(char *msg)
{
    printf("\033[1m%s [%d] \033[1;31merror:\033[0m\033[1m %s in\033[0m\n", file_name, yylineno, msg);
    printf("%s\n", file_content[yylineno -1 ]);
    int index = strstr(file_content[yylineno - 1], yytext) - file_content[yylineno - 1];
    
    printf("\033[1;32m");
    for(int i = 0;i < index;i ++)
        putchar(' ');
    for(int j = 0;j < strlen(yytext);j ++)
        putchar('^');
    putchar('\n');
    printf("\033[0m");
    return 0;
}
