%{ 
    #include <stdio.h>
    #include <stdlib.h>
    #include <stdbool.h>
    #include <string.h>
    #include <stdarg.h>
    #include "syntax.h"
    #define INVALID -100
    #define NON_TERMINAL 0
    #define OPERATOR -1
    extern int yylex();
    extern FILE * yyin;
    extern int yylineno;
    extern char * yytext;
    
    // for reading file
    char * file_content[1024];
    char file_name[1024];
    void read_file();
    
    // error print
    bool has_error = false;
    int yyerror(char * msg);
    
    // grammer tree print option
    bool is_to_print = false;
    
    // tree node definitions
    
    Syntax * root = NULL;
    Syntax * create_string_node(int type, char string_value[128]);
    Syntax * create_int_node(int type, int int_value);
    void traverse(TreeNode * node, int depth);
    void print_node(TreeNode * node, int depth);
    
    TreeNode * reduce(char name[128], int count, ...);
%}
%union
{
    int int_value;
    float float_value;
    char * string_value;
    struct Syntax * syntax_value;
}
%token <string_value> CONST WHILE DO IF INCLUDE RETURN TYPE
%token <int_value> INTEGER
%token <float_value> FLOAT
%token <string_value> LESS_OR_EQUAL GREATER_OR_EQUAL AND OR
%token <string_value> IDENTIFIER
%token <string_value> '+' '-' '*' '/' '#' '<' '>' '=' '(' ')' ',' ';' '.' '~' '!'
%type <string_value> relational_operator plus_minus multiply_divide

%%
program:
        ext_definition_list
        ;

ext_definition_list:
        ext_definition ext_definition_list
        |
        ;

ext_def:
        specifier ext_declaration_list ';'
        |
        specifier ';'
        |
        specifier function_declaration complex_statement
        ;

ext_declaration_list:
        variable_declaration
        |
        variable_declaration ',' ext_declaration_list
        ;

specifier:
        type
        |
        struct_specifier
        ;

struct_specifier:
        STRUCT optional_tag '{' definition_list '}'
        |
        STRUCT IDENTIFIER
        ;

optional_tag:
        IDENTIFIER
        |
        ;

variable_declaration:
        IDENTIFIER
        |
        variable_declaration '[' INTEGER ']'
        ;

function_declaration:
        IDENTIFIER '(' variable_list ')'
        |
        IDENTIFIER '(' ')'
        ;

variable_list:
        parameter_declaration ',' variable_list
        |
        parameter_declaration
        ;

complex_statement:
        '{' definition_list statement_list '}'
        ;

statement_list:
        statement statement_list
        |
        ;

statement:
        expression ';'
        |
        complex_statement
        |
        RETURN expression ';'
        |
        IF '(' expression ')' statement
        |
        IF '(' expression ')' ELSE statement
        |
        WHILE '(' expression ')' statement
        ;

definition_list:
        definition definition_list
        |
        ;

definition:
        specifier declaration_list ';'
        ;

declaration_list:
        declaration
        |
        declaration ',' declaration_list
        ;

declaration:
        variable_declaration
        |
        variable_declaration '=' expression
        ;

relational_operator:
    |'#'
    |'<'
    |LESS_OR_EQUAL
    |EQUAL
    |NOT_EQUAL
    |'>'
    |GREATER_OR_EQUAL
    ;

expression:
        expression '=' expression
        |
        expression AND expression
        |
        expression OR expression
        |
        expression relational_operator expression
        |
        expression '+' expression
        |
        expression '-' expression
        |
        expression '*' expression
        |
        expression '\\' expression
        |
        '(' expression ')'
        |
        '-' expression
        |
        '!' expression
        |
        IDENTIFIER '(' argument_list ')'
        |
        IDENTIFIER '(' ')' 
        |
        expression '(' expression ')'
        |
        expression '.' IDENTIFIER
        |
        IDENTIFIER
        |
        INTEGER
        |
        FLOAT
        ;

argument_list:
        expression ',' argument_list
        |
        expression
        ;

%%
void read_file()
{
    for(int i = 0; !feof(yyin); i++)
    {
        file_content[i] = (char * )malloc(1024 * sizeof(char));
        fgets(file_content[i], 1024, yyin);
        file_content[i][strlen(file_content[i]) - 1] = '\0';
    }
    fseek(yyin, 0, 0);
}

int yyerror(char *msg)
{
    char buf[1024];
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
    
    has_error = true;
    return 0;
}
