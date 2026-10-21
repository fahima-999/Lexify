/* Bison grammar reference; runtime parser is parser.tab.cpp. */
%token INT DOUBLE COUT RETURN USING NAMESPACE STD MAIN ID NUM SHL
%left '+' '-'
%left '*' '/'
%right UMINUS
%%
program: using_opt INT MAIN '(' ')' '{' statements '}' ;
using_opt: /* empty */ | USING NAMESPACE STD ';' ;
statements: /* empty */ | statements statement ;
statement: type ID ';'
         | type ID '=' expr ';'
         | ID '=' expr ';'
         | COUT SHL expr ';'
         | RETURN expr ';'
         ;
type: INT | DOUBLE ;
expr: NUM | ID | '(' expr ')' | '-' expr %prec UMINUS
    | expr '+' expr | expr '-' expr | expr '*' expr | expr '/' expr ;
%%
