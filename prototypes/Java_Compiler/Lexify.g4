// ANTLR reference grammar (not compiled). The standalone project uses Parser.java directly.
grammar Lexify;

program   : PUBLIC? CLASS IDENT '{'
              PUBLIC STATIC VOID MAIN '(' STRING '[' ']' IDENT ')' '{' statement* '}'
            '}' EOF ;
statement : type IDENT ('=' expr)? ';'
          | IDENT '=' expr ';'
          | SYSTEM '.' OUT '.' PRINTLN '(' expr ')' ';'
          | RETURN ';' ;
type      : INT | DOUBLE ;
expr      : '-' expr
          | expr ('*' | '/') expr
          | expr ('+' | '-') expr
          | '(' expr ')'
          | NUMBER
          | IDENT ;

INT: 'int'; DOUBLE: 'double'; RETURN: 'return'; CLASS: 'class'; PUBLIC: 'public'; STATIC: 'static';
VOID: 'void'; MAIN: 'main'; STRING: 'String'; SYSTEM: 'System'; OUT: 'out'; PRINTLN: 'println';
IDENT  : [a-zA-Z_][a-zA-Z0-9_]* ;
NUMBER : [0-9]+ ('.' [0-9]*)? ;
WS     : [ \t\r\n]+ -> skip ;
LINE_COMMENT  : '//' ~[\r\n]* -> skip ;
BLOCK_COMMENT : '/*' .*? '*/' -> skip ;
