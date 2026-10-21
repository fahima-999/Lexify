/* Flex reference scanner for Lexify. The standalone project uses lex.yy.cpp directly.
 * Build it separately to just print tokens:  flex lexer.l && gcc lex.yy.c -o scan */
%option noyywrap yylineno
%{
#include <stdio.h>
%}

D   [0-9]
L   [a-zA-Z_]

%%
"//".*                                   { /* line comment */ }
"/*"([^*]|\*+[^*/])*\*+"/"               { /* block comment */ }
^[ \t]*"#".*                             { printf("PREPROCESSOR %s\n", yytext); }

"int"|"char"|"float"|"double"|"long"|"void"          { printf("TYPE %s\n", yytext); }
"if"|"else"|"while"|"do"|"for"|"switch"|"case"|"default"|"break"|"continue"|"return" {
                                           printf("KEYWORD %s\n", yytext); }
"print"|"printf"|"scanf"                 { printf("BUILTIN %s\n", yytext); }

{L}({L}|{D})*                            { printf("ID %s\n", yytext); }

0[xX][0-9a-fA-F]+[lLuU]*                 { printf("INT_LIT %s\n", yytext); }
{D}+[lLuU]*                              { printf("INT_LIT %s\n", yytext); }
{D}+"."{D}*([eE][+-]?{D}+)?[fFlL]?       { printf("FLOAT_LIT %s\n", yytext); }
"."{D}+([eE][+-]?{D}+)?[fFlL]?           { printf("FLOAT_LIT %s\n", yytext); }
{D}+[eE][+-]?{D}+[fFlL]?                 { printf("FLOAT_LIT %s\n", yytext); }

'(\\.|[^\\'\n])'                         { printf("CHAR_LIT %s\n", yytext); }
\"(\\.|[^\\"\n])*\"                      { printf("STRING %s\n", yytext); }

"<<="|">>="                              { printf("OP %s\n", yytext); }
"++"|"--"|"+="|"-="|"*="|"/="|"%="|"&="|"|="|"^="|"<<"|">>"|"<="|">="|"=="|"!="|"&&"|"||" {
                                           printf("OP %s\n", yytext); }
[-+*/%=<>!~&|^?:;,(){}]                  { printf("OP %s\n", yytext); }

[ \t\r\n]+                               { /* whitespace */ }
.                                        { printf("LEXICAL ERROR line %d: unexpected '%s'\n", yylineno, yytext); }
%%

int main(void) { yylex(); return 0; }
