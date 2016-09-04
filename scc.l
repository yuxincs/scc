%{
#include <stdio.h>
#include <stdlib.h>
#include "y.tab.h"
%}
%option yylineno
digit            [0-9]
number           {digit}+
letter           [a-zA-Z]
identifier       {letter}({digit}|{letter}|_)*
blank            [\t\n\x0B\f\r\x20]+
notes            \/\*.*\*\/
%%
#include             { return INCLUDE; }
#[^\n]*              { /* Discard preprocessor comments. */ }
const                { yylval.string_value = strdup(yytext); return CONST;}
if                   { yylval.string_value = strdup(yytext); return IF;}
while                { yylval.string_value = strdup(yytext); return WHILE;}
do                   { yylval.string_value = strdup(yytext); return DO;}
return               { yylval.string_value = strdup(yytext); return RETURN; }
int                  { yylval.string_value = strdup(yytext); return TYPE; }
float                { yylval.string_value = strdup(yytext); return TYPE; }

{identifier}         { yylval.string_value = strdup(yytext); return IDENTIFIER;}

{number}             { yylval.int_value = atoi(yytext); return INTEGER;}

[<][=]               { yylval.string_value = strdup(yytext); return LESS_OR_EQUAL;}
[>][=]               { yylval.string_value = strdup(yytext); return GREATER_OR_EQUAL;}
[-+\*/#<>=\(\),;\.]  { yylval.string_value = strdup(yytext); return yytext[0];}

{blank}              { /* Do nothing */  }

{notes}              { /* Do nothing */ }

.                    { return 0; }

%%

int yywrap()
{
	return 1;
}

