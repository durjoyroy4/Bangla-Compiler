
%option noyywrap yylineno

%{
#include "parser.tab.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
%}

DIGIT       [0-9]
LETTER      [a-zA-Z_]
IDENTIFIER  {LETTER}({LETTER}|{DIGIT})*

%%

"dhoro"         { return DHORO; }
"dekhao"        { return DEKHAO; }
"shuno"         { return SHUNO; }
"jodi"          { return JODI; }
"noyto"         { return NOYTO; }
"jotokkhon"     { return JOTOKKHON; }

"=="            { return EQ; }
"!="            { return NE; }
"<="            { return LE; }
">="            { return GE; }
"&&"            { return AND; }
"||"            { return OR; }

"+"             { return PLUS; }
"-"             { return MINUS; }
"*"             { return MULTIPLY; }
"/"             { return DIVIDE; }
"%"             { return MODULO; }

"="             { return ASSIGN; }
"<"             { return LT; }
">"             { return GT; }
"!"             { return NOT; }

";"             { return SEMICOLON; }
"("             { return LPAREN; }
")"             { return RPAREN; }
"{"             { return LBRACE; }
"}"             { return RBRACE; }

{DIGIT}+"."{DIGIT}+ {
    yylval.float_value = strtod(yytext, NULL);
    return FLOAT_LITERAL;
}

{DIGIT}+ {
    yylval.int_value = (int)strtol(yytext, NULL, 10);
    return INT_LITERAL;
}

{IDENTIFIER} {
    yylval.string = strdup(yytext);
    return IDENTIFIER;
}

"//".*          { }

[ \t\r\n]+      { }

. {
    fprintf(stderr,
        "Lexical Error: Unknown character '%s' at line %d\n",
        yytext, yylineno);
    return INVALID;
}

%%
