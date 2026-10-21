# Lexify — Mini Compiler

A functional educational compiler/interpreter for a **small C-like subset**, not a full C compiler.

## Features
- Lexical analysis (tokens)
- Recursive-descent parsing and AST
- Semantic validation (declared variables, duplicate declarations)
- Integer arithmetic and assignments
- Three-address code (TAC)
- Execution of `print(expression);`

## Build
From `C_Compiler`:

```bash
g++ -std=c++17 -Wall -Wextra main.cpp ast.cpp assembly.cpp codegen.cpp interpreter.cpp lex.yy.cpp parser.tab.cpp semantic.cpp symbol_table.cpp tac.cpp -o lexify
```

On Linux/macOS you can alternatively run `make`. On Windows with MinGW, use the g++ command (output `lexify.exe` if desired).

## Run
```bash
./lexify input.c
./lexify input.c --tokens
./lexify input.c --tac
```
On Windows: `lexify.exe input.c`.

Expected output:
```
20
8
```

## Supported grammar
```
statement := 'int' ID '=' expr ';' | ID '=' expr ';' | 'print' '(' expr ')' ';'
expr := term (('+' | '-') term)*
term := factor (('*' | '/') factor)*
factor := NUM | ID | '(' expr ')' | '-' factor
```

## Limitations
No functions, loops, conditionals, pointers, strings, floating point, or full C support. `lexer.l` and `parser.y` are optional teaching references; the actual build uses the handwritten `lex.yy.cpp` and `parser.tab.cpp`. `assembly.cpp` only formats TAC as comments, not machine code. The frontend is a static preview; the backend and other language modules are future-work placeholders. `compiler.exe` is not included because executables should be compiled for your operating system.
