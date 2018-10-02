%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "syntax.h"
#include "scc_yacc.h"
%}
%option yylineno
integer                      [1-9]+[0-9]*|0
float                        {integer}\.[0-9]+
letter                       [a-zA-Z]
identifier                   ({letter}|_)([0-9]|{letter}|_)*
blank                        [\t\n\x0B\f\r\x20]+
notes                        \/\*.*\*\/|\/\/.*
type                         int|float
%%
#[^\n]*                      { /* Discard preprocessor comments. */ }
else                         { return L_ELSE;}
if                           { return L_IF;}
while                        { return L_WHILE;}
struct                       { return L_STRUCT;}
return                       { return L_RETURN; }
void                         { return L_VOID; }

{type}                       { yylval.string_value = strdup(yytext); return L_BASIC_TYPE; }
{identifier}                 { yylval.string_value = strdup(yytext); return L_IDENTIFIER;}
{integer}                    { yylval.int_value = atoi(yytext); return L_INTEGER;}
{float}                      { yylval.float_value = atof(yytext); return L_FLOAT;}

[=][=]                       { return L_EQUAL;}
[!][=]                       { return L_NOT_EQUAL;}
[<][=]                       { return L_LESS_OR_EQUAL;}
[>][=]                       { return L_GREATER_OR_EQUAL;}
[|][|]                       { return L_OR;}
[&][&]                       { return L_AND;}
[-+\*/#<>=\(\),;\.~!{}\]\[]  { return yytext[0];}

{blank}                      { /* Do nothing */  }

{notes}                      { /* Do nothing */ }

.                            { return 0; }

%%

int yywrap()
{
    return 1;
}

