/* Lexify parser (Bison) - grammar from Section 5 of the project manual.
 * This is the syntax-only skeleton: actions to build the AST go in the
 * rules below (see src/ast/). */
%code requires {
#include "ast/ast.h"
}

%{
#include <stdio.h>
#include <stdlib.h>
#include "ast/ast.h"

int  yylex(void);
void yyerror(const char *msg);
extern int yylineno;
int   syntax_errors = 0;
Node *ast_root = NULL;
%}

%define parse.error verbose
%locations

%union {
    int    ival;
    double fval;
    char  *sval;
    Type   type;
    Node  *node;
}

%token INT FLOAT BOOL IF ELSE WHILE PRINT TRUE_LIT FALSE_LIT
%token <sval> ID
%token <ival> INT_LIT
%token <fval> FLOAT_LIT
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

%type <node> program stmt_list stmt block decl expr
%type <type> type

%%

program     : stmt_list                 { ast_root = mk_program($1); $$ = ast_root; }
            ;

stmt_list   : /* empty */               { $$ = NULL; }
            | stmt_list stmt            { $$ = list_append($1, $2); }
            ;

stmt        : decl ';'                  { $$ = $1; }
            | ID '=' expr ';'           { $$ = mk_assign($1, $3, @1.first_line); }
            | PRINT expr ';'            { $$ = mk_print($2, @1.first_line); }
            | IF '(' expr ')' stmt %prec LOWER_THAN_ELSE
                                        { $$ = mk_if($3, $5, NULL, @1.first_line); }
            | IF '(' expr ')' stmt ELSE stmt
                                        { $$ = mk_if($3, $5, $7, @1.first_line); }
            | WHILE '(' expr ')' stmt   { $$ = mk_while($3, $5, @1.first_line); }
            | block                     { $$ = $1; }
            | error ';'                 { yyerrok; $$ = NULL; }
            ;

block       : '{' stmt_list '}'         { $$ = mk_block($2, @1.first_line); }
            ;

decl        : type ID                   { $$ = mk_decl($1, $2, @2.first_line); }
            ;

type        : INT                       { $$ = T_INT; }
            | FLOAT                     { $$ = T_FLOAT; }
            | BOOL                      { $$ = T_BOOL; }
            ;

expr        : expr OR expr              { $$ = mk_binop("||", $1, $3, @2.first_line); }
            | expr AND expr             { $$ = mk_binop("&&", $1, $3, @2.first_line); }
            | expr EQ expr              { $$ = mk_binop("==", $1, $3, @2.first_line); }
            | expr NEQ expr             { $$ = mk_binop("!=", $1, $3, @2.first_line); }
            | expr '<' expr             { $$ = mk_binop("<",  $1, $3, @2.first_line); }
            | expr '>' expr             { $$ = mk_binop(">",  $1, $3, @2.first_line); }
            | expr LE expr              { $$ = mk_binop("<=", $1, $3, @2.first_line); }
            | expr GE expr              { $$ = mk_binop(">=", $1, $3, @2.first_line); }
            | expr '+' expr             { $$ = mk_binop("+",  $1, $3, @2.first_line); }
            | expr '-' expr             { $$ = mk_binop("-",  $1, $3, @2.first_line); }
            | expr '*' expr             { $$ = mk_binop("*",  $1, $3, @2.first_line); }
            | expr '/' expr             { $$ = mk_binop("/",  $1, $3, @2.first_line); }
            | expr '%' expr             { $$ = mk_binop("%",  $1, $3, @2.first_line); }
            | '!' expr                  { $$ = mk_unop("!", $2, @1.first_line); }
            | '(' expr ')'              { $$ = $2; }
            | ID                        { $$ = mk_id($1, @1.first_line); }
            | INT_LIT                   { $$ = mk_int($1, @1.first_line); }
            | FLOAT_LIT                 { $$ = mk_float($1, @1.first_line); }
            | TRUE_LIT                  { $$ = mk_bool(1, @1.first_line); }
            | FALSE_LIT                 { $$ = mk_bool(0, @1.first_line); }
            ;

%%

void yyerror(const char *msg) {
    fprintf(stderr, "Syntax error at line %d: %s\n", yylineno, msg);
    syntax_errors++;
}
