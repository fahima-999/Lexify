/* Flex reference grammar. The standalone project uses lex.yy.cpp directly. */
%option noyywrap yylineno
%x COMMENT
%{
#include <stdio.h>
%}
%%
"int"|"double"|"cout"|"return"|"using"|"namespace"|"std"|"main" { printf("KEYWORD %s\n",yytext); }
[a-zA-Z_][a-zA-Z0-9_]*  { printf("ID %s\n",yytext); }
[0-9][0-9.]*            { printf("NUM %s\n",yytext); }
"<<"|"::"               { printf("SYMBOL %s\n",yytext); }
[-+*/=;(){}<>,]         { printf("SYMBOL %s\n",yytext); }
"//"[^\n]*              ;
"/*"                    { BEGIN(COMMENT); }
<COMMENT>"*/"           { BEGIN(INITIAL); }
<COMMENT>.|\n           ;
[ \t\r\n]+              ;
.                       { printf("ERROR unexpected character %s at line %d\n",yytext,yylineno); }
%%
