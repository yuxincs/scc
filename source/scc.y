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
    char * string_value;
    struct tree_node * node_value;
}
%token <string_value> CONST WHILE DO IF INCLUDE RETURN TYPE
%token <int_value> INTEGER
%token <string_value> LESS_OR_EQUAL GREATER_OR_EQUAL
%token <string_value> IDENTIFIER
%token <string_value> '+' '-' '*' '/' '#' '<' '>' '=' '(' ')' ',' ';' '.' '~' '!'
%type <string_value> relational_operator plus_minus multiply_divide
%type <node_value> program sub_program const_statement var_statement  statement
%type <node_value> const_declaration_list const_declaration identifier_list 
%type <node_value> factor if_statement call_statement while_statement read_statement assign_statement
%type <node_value> factor_list item_list expression condition empty_statement write_statement
%type <node_value> complex_statement procedure_statement statement_list expression_list procedure_head
%%
program: 
    {
        root = create_string_node(NON_TERMINAL, "Program");
        if(is_to_print)
            traverse(root, 0);
    }
    |sub_program '.' 
    {
        TreeNode * dot = create_string_node('.', ".");
        root = reduce("Program", 2, $1, dot);
        if(is_to_print)
            traverse(root, 0);
    }
    ;
    
sub_program: const_statement var_statement procedure_statement statement 
    {
        $$ = reduce("SubProgram", 4, $1, $2, $3, $4);
    }
    ;
    
const_statement: 
    {
        $$ = NULL;
    }
    | CONST const_declaration_list ';' 
    {
        TreeNode * const_node = create_string_node(CONST, "const");
        TreeNode * colon_node = create_string_node(';', ";");
        $$ = reduce("ConstStatement", 3, const_node, $2, colon_node);
    }
    ;
    
const_declaration_list: const_declaration 
    {
        $$ = $1;
    }
    | const_declaration_list ',' const_declaration 
    {
        TreeNode * comma_node = create_string_node(',', ",");
        $$ = reduce("ConstDeclarationList", 3, $1, comma_node, $3);
    }
    ;
    
const_declaration: IDENTIFIER '=' INTEGER
    {
        TreeNode * identifier = create_string_node(IDENTIFIER, $1);
        TreeNode * equal = create_string_node('=', "=");
        TreeNode * integer = create_int_node(INTEGER, $3);
        $$ = reduce("ConstDeclaration", 3, identifier, equal, integer);
    }
    ;
    
var_statement:
    {
        $$ = NULL;
    }
    |VAR identifier_list ';'
    {
        TreeNode * var_node = create_string_node(VAR, "var");
        TreeNode * colon_node = create_string_node(';', ";");
        $$ = reduce("VarStatement", 3, var_node, $2, colon_node);    
    }
    ;
    
identifier_list: IDENTIFIER 
    {
        TreeNode * identifier_node = create_string_node(IDENTIFIER, $1);
        $$ = identifier_node;
    }
    | identifier_list ',' IDENTIFIER
    {
        TreeNode * identifier_node = create_string_node(IDENTIFIER, $3);
        TreeNode * comma_node = create_string_node(',', ",");
        $$ = reduce("IdentifierList", 3, $1, comma_node, identifier_node);
    }
    ;
    
procedure_statement:
    {
        $$ = NULL;
    }
    |procedure_head sub_program ';' procedure_statement
    {
        TreeNode * colon_node = create_string_node(';', ";");
        $$ = reduce("ProcedureStatement", 4, $1, $2, colon_node, $4);
    }
    ;
    
procedure_head: PROCEDURE IDENTIFIER ';'
    {
        TreeNode * procedure_node = create_string_node(PROCEDURE, "procedure");
        TreeNode * identifier_node = create_string_node(IDENTIFIER, $2);
        TreeNode * colon_node = create_string_node(';', ";");
        $$ = reduce("ProcedureHead", 3, procedure_node, identifier_node, colon_node);
    }
    ;
    
statement: assign_statement
    |if_statement
    |while_statement
    |call_statement
    |read_statement
    |write_statement
    |complex_statement
    |empty_statement
    ;
    
assign_statement: IDENTIFIER BECOMES expression
    {
        TreeNode * identifier_node = create_string_node(IDENTIFIER, $1);
        TreeNode * becomes_node = create_string_node(BECOMES, ":=");
        $$ = reduce("AssignStatement", 3, identifier_node, becomes_node, $3);
    }
    ;
    
complex_statement: BEGINN statement_list END
    {
        TreeNode * begin_node = create_string_node(BEGINN, "begin");
        TreeNode * end_node = create_string_node(END, "end");
        $$ = reduce("ComplexStatement", 3, begin_node, $2, end_node);
    }
    ;
    
statement_list: statement 
    {
        $$ = $1;
    }
    | statement_list ';' statement
    {
        TreeNode * colon_node = create_string_node(';', ";");
        $$ = reduce("StatementList", 3, $1, colon_node, $3);
    }
    ;
    
empty_statement:
    {
        $$ = NULL;
    }
    ;
    
condition: expression relational_operator expression
    {
        TreeNode * relational_operator = create_string_node(OPERATOR, $2);
        $$ = reduce("Condition", 3, $1, relational_operator, $3);
    }
    |ODD expression
    {
        TreeNode * odd_node = create_string_node(ODD, "odd");
        $$ = reduce("Condition", 2, odd_node, $2);
    }
    ;
    
expression: item_list
    {
        $$ = $1;
    }
    | plus_minus item_list
    {
        TreeNode * plus_minus = create_string_node(OPERATOR, $1);
        $$ = reduce("Expression", 2, plus_minus, $2);
    }
    ;
    
item_list: factor_list 
    {
        $$ = $1;
    }
    | item_list plus_minus factor_list
    {
        TreeNode * plus_minus = create_string_node(OPERATOR, $2);
        $$ = reduce("ItemList", 3, $1, plus_minus, $3);
    }
    ;
    
    
factor_list:factor 
    {
        $$ = $1;
    }
    |factor_list multiply_divide factor
    {
        TreeNode * multiply_divide = create_string_node(OPERATOR, $2);
        $$ = reduce("FactorList", 3, $1, multiply_divide, $3);
    }
    ;
    
factor: IDENTIFIER
    {
        TreeNode * identifier_node = create_string_node(IDENTIFIER, $1);
        $$ = identifier_node;
    }
    |INTEGER
    {
        TreeNode * integer_node = create_int_node(INTEGER, $1);
        $$ = integer_node;
    }
    |'(' expression ')'
    {
        TreeNode * lbrace_node = create_string_node('(', "(");
        TreeNode * rbrace_node = create_string_node(')', ")");
        $$ = reduce("Factor", 3, lbrace_node, $2, rbrace_node);
    }
    ;
    
plus_minus: '+'
    |'-'
    ;
    
multiply_divide: '*'
    |'/'
    ;
    
relational_operator: '='
    |'#'
    |'<'
    |LE
    |'>'
    |GE
    ;
    
if_statement: IF condition THEN statement 
    {
        TreeNode * if_node = create_string_node(IF, "if");
        TreeNode * then_node = create_string_node(THEN, "then");
        $$ = reduce("IfStatement", 4, if_node, $2, then_node, $4);
    }
    ;
    
call_statement: CALL IDENTIFIER
    {
        TreeNode * call_node = create_string_node(CALL, "call");
        TreeNode * identifier_node = create_string_node(IDENTIFIER, $2);
        $$ = reduce("CallStatement", 2, call_node, identifier_node);
    }
    ;
    
while_statement: WHILE condition DO statement
    {
        TreeNode * while_node = create_string_node(WHILE, "while");
        TreeNode * do_node = create_string_node(DO, "do");
        $$ = reduce("WhileStatement", 4, while_node, $2, do_node, $4);
    }
    ;
    
read_statement: READ '(' identifier_list ')'
    {
        TreeNode * read_node = create_string_node(READ, "read");
        TreeNode * lbrace_node = create_string_node('(', "(");
        TreeNode * rbrace_node = create_string_node(')', ")");
        $$ = reduce("ReadStatement", 4, read_node, lbrace_node, $3, rbrace_node);
    }
    ;
    
write_statement: WRITE '(' expression_list ')' 
    {
        TreeNode * write_node = create_string_node(WRITE, "write");
        TreeNode * lbrace_node = create_string_node('(', "(");
        TreeNode * rbrace_node = create_string_node(')', ")");
        $$ = reduce("WriteStatement", 4, write_node, lbrace_node, $3, rbrace_node);
    }
    ;
    
expression_list: expression 
    {
        $$ = NULL;
    }
    | expression_list ',' expression 
    {
        TreeNode * comma_node = create_string_node(',', ",");
        $$ = reduce("ExpressionList", 3, $1, comma_node, $3);
    }
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
