%{
#include <stdio.h>
#include <stdlib.h>
#include "y.tab.h"
%}
%option yylineno
digit             [0-9]
integer           {digit}+
float             {digit}+\.{digit}*
letter            [a-zA-Z]
identifier        {letter}({digit}|{letter}|_)*
blank             [\t\n\x0B\f\r\x20]+
notes             \/\*.*\*\/
type              int|float
%%
#[^\n]*                { /* Discard preprocessor comments. */ }
const                  { yylval.string_value = strdup(yytext); return CONST;}
if                     { yylval.string_value = strdup(yytext); return IF;}
while                  { yylval.string_value = strdup(yytext); return WHILE;}
struct                 { yylval.string_value = strdup(yytext); return STRUCT;}
return                 { yylval.string_value = strdup(yytext); return RETURN; }

{type}                 { yylval.string_value = strdup(yytext); return TYPE; }
{identifier}           { yylval.string_value = strdup(yytext); return IDENTIFIER;}
{integer}               { yylval.int_value = atoi(yytext); return INTEGER;}
{number}               { yylval.int_value = atoi(yytext); return INTEGER;}

[=][=]                 { yylval.string_value = strdup(yytext); return EQUAL;}
[!][=]                 { yylval.string_value = strdup(yytext); return NOT_EQUAL;}
[<][=]                 { yylval.string_value = strdup(yytext); return LESS_OR_EQUAL;}
[>][=]                 { yylval.string_value = strdup(yytext); return GREATER_OR_EQUAL;}
[|][|]                 { yylval.string_value = strdup(yytext); return OR;}
[&][&]                 { yylval.string_value = strdup(yytext); return AND;}
[-+\*/#<>=\(\),;\.~!]  { yylval.string_value = strdup(yytext); return yytext[0];}

{blank}                { /* Do nothing */  }

{notes}                { /* Do nothing */ }

.                      { return 0; }

%%

int yywrap()
{
    return 1;
}

