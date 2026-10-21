/* Bison grammar reference; runtime parser is parser.tab.cpp. */
%token INT PRINT ID NUM
%left '+' '-'
%left '*' '/'
%%
program: /* empty */ | program statement ;
statement: INT ID '=' expr ';' | ID '=' expr ';' | PRINT '(' expr ')' ';' ;
expr: NUM | ID | '(' expr ')' | '-' expr | expr '+' expr | expr '-' expr | expr '*' expr | expr '/' expr ;
%%
