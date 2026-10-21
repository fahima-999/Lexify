# Formal CFG (matches src/parser/parser.y)

```
program   -> stmt_list
stmt_list -> ε | stmt_list stmt
stmt      -> decl ';'
           | ID '=' expr ';'
           | 'print' expr ';'
           | 'if' '(' expr ')' stmt
           | 'if' '(' expr ')' stmt 'else' stmt
           | 'while' '(' expr ')' stmt
           | block
block     -> '{' stmt_list '}'
decl      -> type ID
type      -> 'int' | 'float' | 'bool'
expr      -> expr '||' expr | expr '&&' expr
           | expr ('==' | '!=') expr
           | expr ('<' | '>' | '<=' | '>=') expr
           | expr ('+' | '-') expr
           | expr ('*' | '/' | '%') expr
           | '!' expr | '(' expr ')'
           | ID | INT_LIT | FLOAT_LIT | 'true' | 'false'
```

Precedence (low to high): `||`, `&&`, `== !=`, `< > <= >=`, `+ -`, `* / %`, `!`. All binary operators are left-associative.
Dangling else is resolved by `%prec LOWER_THAN_ELSE` (else binds to the nearest if).
