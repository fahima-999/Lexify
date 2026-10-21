/* Flex reference grammar. The standalone project uses lex.yy.cpp directly. */
%option noyywrap
%{
#include <stdio.h>
%}
%%
"int"|"print"          { printf("KEYWORD %s\n",yytext); }
[a-zA-Z_][a-zA-Z0-9_]* { printf("ID %s\n",yytext); }
[0-9]+                 { printf("NUM %s\n",yytext); }
[ \t\r\n]+             ;
"//"[^\n]*              ;
.                       { printf("TOKEN %s\n",yytext); }
%%
