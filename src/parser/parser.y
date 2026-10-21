/* Lexify parser (Bison) - grammar from Section 5 of the project manual.
 * This is the syntax-only skeleton: actions to build the AST go in the
 * rules below (see src/ast/). */
%{
#include <stdio.h>

int  yylex(void);
void yyerror(const char *msg);
extern int yylineno;
int syntax_errors = 0;
%}

%define parse.error verbose

%token INT FLOAT BOOL IF ELSE WHILE PRINT TRUE_LIT FALSE_LIT
%token ID INT_LIT FLOAT_LIT
%token AND OR EQ NEQ LE GE

%left OR
%left AND
%left EQ NEQ
%left '<' '>' LE GE
%left '+' '-'
%left '*' '/' '%'
%right '!'
%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE

%%

program     : stmt_list ;

stmt_list   : /* empty */
            | stmt_list stmt
            ;

stmt        : decl ';'
            | ID '=' expr ';'
            | PRINT expr ';'
            | IF '(' expr ')' stmt %prec LOWER_THAN_ELSE
            | IF '(' expr ')' stmt ELSE stmt
            | WHILE '(' expr ')' stmt
            | block
            | error ';'   { yyerrok; }
            ;

block       : '{' stmt_list '}' ;

decl        : type ID ;

type        : INT | FLOAT | BOOL ;

expr        : expr OR expr
            | expr AND expr
            | expr EQ expr
            | expr NEQ expr
            | expr '<' expr
            | expr '>' expr
            | expr LE expr
            | expr GE expr
            | expr '+' expr
            | expr '-' expr
            | expr '*' expr
            | expr '/' expr
            | expr '%' expr
            | '!' expr
            | '(' expr ')'
            | ID
            | INT_LIT
            | FLOAT_LIT
            | TRUE_LIT
            | FALSE_LIT
            ;

%%

void yyerror(const char *msg) {
    fprintf(stderr, "Syntax error at line %d: %s\n", yylineno, msg);
    syntax_errors++;
}
