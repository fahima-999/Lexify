/* JFlex reference grammar (not compiled). The standalone project uses Lexer.java directly. */
%%
%class RefLexer
%unicode
%line
%standalone
%%
"int"|"double"|"return"|"class"|"public"|"static"|"void"|"main"|"String"|"System"|"out"|"println" { System.out.println("KEYWORD " + yytext()); }
[a-zA-Z_][a-zA-Z0-9_]*   { System.out.println("IDENT " + yytext()); }
[0-9]+(\.[0-9]*)?        { System.out.println("NUMBER " + yytext()); }
[ \t\r\n]+               { /* skip whitespace */ }
"//" [^\n]*              { /* skip line comment */ }
"/*" ~"*/"               { /* skip block comment */ }
[-+*/=;(){}\[\].]        { System.out.println("TOKEN " + yytext()); }
.                        { System.out.println("ERROR unexpected character " + yytext() + " at line " + (yyline + 1)); }
