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

    // top level syntax definition exists in main.c
    extern Syntax * top_level;
    
    char file_name[1024];
    char *file_content[1024];
    int yyerror(char * msg);
%}
%union
{
    int int_value;
    float float_value;
    char * string_value;
    Syntax * syntax_value;
}
%token CONST WHILE DO IF INCLUDE RETURN ELSE STRUCT
%token LESS_OR_EQUAL GREATER_OR_EQUAL AND OR EQUAL NOT_EQUAL
%token <int_value> INTEGER
%token <float_value> FLOAT_NUMBER
%token <string_value> IDENTIFIER TYPE
%token '+' '-' '*' '/' '#' '<' '>' '=' '(' ')' ',' ';' '.' '~' '!'
%type <syntax_value> program external_definition_list external_definition external_declaration_list
%type <syntax_value> specifier struct_specifier optional_tag variable_declaration
%type <syntax_value> function_declaration variable_list
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
        external_definition_list
        {
            top_level = (Syntax *)malloc(sizeof(Syntax));
            top_level->type = TOP_LEVEL;
            top_level->top_level->declarations = list_create();
        }
        ;

external_definition_list:
        external_definition external_definition_list
        {
            
        }
        |
        {
            
        }
        ;

external_definition:
        specifier external_declaration_list ';'
        {

        }
        |
        specifier ';'
        {

        }
        |
        specifier function_declaration complex_statement
        {

        }
        ;

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

        }
        |
        struct_specifier
        {

        }
        ;

struct_specifier:
        STRUCT optional_tag '{' definition_list '}'
        {

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

variable_declaration:
        IDENTIFIER
        {

        }
        |
        variable_declaration '[' INTEGER ']'
        {

        }
        ;

function_declaration:
        IDENTIFIER '(' variable_list ')'
        {

        }
        |
        IDENTIFIER '(' ')'
        {

        }
        ;

variable_list:
        parameter_declaration ',' variable_list
        {

        }
        |
        parameter_declaration
        {

        }
        ;

parameter_declaration:
        specifier variable_declaration
        {

        }
        ;

complex_statement:
        '{' definition_list statement_list '}'
        {

        }
        ;

statement_list:
        statement statement_list
        {

        }
        |
        ;

statement:
        expression ';'
        {

        }
        |
        complex_statement
        {

        }
        |
        RETURN expression ';'
        {

        }
        |
        IF '(' expression ')' statement
        {

        }
        |
        IF '(' expression ')' ELSE statement
        {

        }
        |
        WHILE '(' expression ')' statement
        {

        }
        ;

definition_list:
        definition definition_list
        {

        }
        |
        ;

definition:
        specifier declaration_list ';'
        {

        }
        ;

declaration_list:
        declaration
        {

        }
        |
        declaration ',' declaration_list
        {

        }
        ;

declaration:
        variable_declaration
        {

        }
        |
        variable_declaration '=' expression
        {

        }
        ;

relational_operator:
        |
        '#'
        |
        '<'
        |
        LESS_OR_EQUAL
        |
        EQUAL
        |
        NOT_EQUAL
        |
        '>'
        |
        GREATER_OR_EQUAL
        ;

expression:
        expression '=' expression
        {

        }
        |
        expression AND expression
        {

        }
        |
        expression OR expression
        {

        }
        |
        expression relational_operator expression
        {

        }
        |
        expression '+' expression
        {

        }
        |
        expression '-' expression
        {

        }
        |
        expression '*' expression
        {

        }
        |
        expression '\\' expression
        {

        }
        |
        '(' expression ')'
        {

        }
        |
        '-' expression
        {

        }
        |
        '!' expression
        {

        }
        |
        IDENTIFIER '(' argument_list ')'
        {

        }
        |
        IDENTIFIER '(' ')'
        {

        } 
        |
        expression '(' expression ')'
        {

        }
        |
        expression '.' IDENTIFIER
        {

        }
        |
        IDENTIFIER
        {

        }
        |
        INTEGER
        {

        }
        |
        FLOAT_NUMBER
        {

        }
        ;

argument_list:
        expression ',' argument_list
        {

        }
        |
        expression
        {

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
