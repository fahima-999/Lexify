/* Bison reference grammar for Lexify (the language: C from declarations up to loops).
 * The runtime parser is the hand written parser.tab.cpp; this file documents the grammar. */
%token INT CHAR FLOAT DOUBLE LONG VOID
%token IF ELSE WHILE DO FOR SWITCH CASE DEFAULT BREAK CONTINUE RETURN
%token PRINT PRINTF SCANF
%token ID INT_LIT FLOAT_LIT CHAR_LIT STRING
%token INC DEC
%token ADD_ASSIGN SUB_ASSIGN MUL_ASSIGN DIV_ASSIGN MOD_ASSIGN AND_ASSIGN OR_ASSIGN XOR_ASSIGN SHL_ASSIGN SHR_ASSIGN
%token SHL SHR LE GE EQ NE AND_OP OR_OP

%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE

%left OR_OP
%left AND_OP
%left '|'
%left '^'
%left '&'
%left EQ NE
%left '<' '>' LE GE
%left SHL SHR
%left '+' '-'
%left '*' '/' '%'

%%
program        : /* empty */
               | program top_item
               ;

top_item       : main_function
               | statement
               ;

main_function  : type_specifier ID '(' ')' block
               | type_specifier ID '(' VOID ')' block
               ;

type_specifier : INT | CHAR | FLOAT | DOUBLE | VOID
               | LONG | LONG LONG | LONG INT | LONG LONG INT
               ;

block          : '{' stmt_list '}' ;
stmt_list      : /* empty */ | stmt_list statement ;

statement      : ';'
               | declaration ';'
               | expr ';'
               | block
               | if_stmt
               | WHILE '(' expr ')' statement
               | DO statement WHILE '(' expr ')' ';'
               | FOR '(' for_init ';' opt_expr ';' opt_expr ')' statement
               | SWITCH '(' expr ')' '{' case_list '}'
               | BREAK ';'
               | CONTINUE ';'
               | RETURN opt_expr ';'
               | PRINT '(' assignment ')' ';'
               | PRINTF '(' STRING printf_args ')' ';'
               | SCANF '(' STRING scanf_args ')' ';'
               ;

if_stmt        : IF '(' expr ')' statement %prec LOWER_THAN_ELSE
               | IF '(' expr ')' statement ELSE statement
               ;

for_init       : /* empty */ | declaration | expr ;
opt_expr       : /* empty */ | expr ;

case_list      : /* empty */
               | case_list case_label
               | case_list statement
               ;
case_label     : CASE conditional ':'
               | DEFAULT ':'
               ;

declaration    : type_specifier init_list ;
init_list      : init_declarator | init_list ',' init_declarator ;
init_declarator: ID | ID '=' assignment ;

printf_args    : /* empty */ | printf_args ',' assignment ;
scanf_args     : /* empty */ | scanf_args ',' '&' ID ;

/* ---------------------------------------------------------- expressions */
expr           : assignment
               | expr ',' assignment
               ;

assignment     : conditional
               | ID assign_op assignment
               ;

assign_op      : '=' | ADD_ASSIGN | SUB_ASSIGN | MUL_ASSIGN | DIV_ASSIGN | MOD_ASSIGN
               | AND_ASSIGN | OR_ASSIGN | XOR_ASSIGN | SHL_ASSIGN | SHR_ASSIGN
               ;

conditional    : binary
               | binary '?' expr ':' conditional
               ;

binary         : unary
               | binary OR_OP binary
               | binary AND_OP binary
               | binary '|' binary
               | binary '^' binary
               | binary '&' binary
               | binary EQ binary
               | binary NE binary
               | binary '<' binary
               | binary '>' binary
               | binary LE binary
               | binary GE binary
               | binary SHL binary
               | binary SHR binary
               | binary '+' binary
               | binary '-' binary
               | binary '*' binary
               | binary '/' binary
               | binary '%' binary
               ;

unary          : postfix
               | '+' unary
               | '-' unary
               | '!' unary
               | '~' unary
               | INC unary
               | DEC unary
               | '(' type_specifier ')' unary
               ;

postfix        : primary
               | postfix INC
               | postfix DEC
               ;

primary        : ID | INT_LIT | FLOAT_LIT | CHAR_LIT
               | '(' expr ')'
               ;
%%
