%{
#include <stdio.h>
#include <stdlib.h>
#include "syntax.h"
#include "scc_yacc.hpp"
%}
%option yylineno
integer                  [1-9]+[0-9]*|0
float                    {integer}\.[0-9]+
letter                   [a-zA-Z]
identifier               {letter}([0-9]|{letter}|_)*
blank                    [\t\n\x0B\f\r\x20]+
notes                    \/\*.*\*\/
type                     int|float
%%
#[^\n]*                  { /* Discard preprocessor comments. */ }
const                    { return CONST;}
else                     { return ELSE;}
if                       { return IF;}
while                    { return WHILE;}
struct                   { return STRUCT;}
return                   { return RETURN; }

{type}                   { yylval.string_value = strdup(yytext); return TYPE; }
{identifier}             { yylval.string_value = strdup(yytext); return IDENTIFIER;}
{integer}                { yylval.int_value = atoi(yytext); return INTEGER;}
{float}                  { yylval.float_value = atof(yytext); return FLOAT_NUMBER;}

[=][=]                   { return EQUAL;}
[!][=]                   { return NOT_EQUAL;}
[<][=]                   { return LESS_OR_EQUAL;}
[>][=]                   { return GREATER_OR_EQUAL;}
[|][|]                   { return OR;}
[&][&]                   { return AND;}
[-+\*/#<>=\(\),;\.~!{}]  { return yytext[0];}

{blank}                  { /* Do nothing */  }

{notes}                  { /* Do nothing */ }

.                        { return 0; }

%%

int yywrap()
{
    return 1;
}

