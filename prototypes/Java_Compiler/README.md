# Lexify Mini Java Compiler

A small **educational Java-subset interpreter**, not a complete Java compiler.
Its layout and features mirror the `CPP_Compiler` project (token stream, AST, recursive-descent `Parser`, typed `SymbolTable`, semantic checks, three-address code, pseudo-assembly, interpreter).

## Supported syntax
- A single class with `public static void main(String[] args) { ... }` (the leading `public` on the class is optional)
- `int` and `double` declarations (with or without an initial value), assignments
- Arithmetic `+ - * /`, parentheses, unary minus
- `System.out.println(expression);` and `return;`
- `//` and `/* ... */` comments
- Lexing, parsing, undeclared/duplicate-variable checks, type checks, three-address code, pseudo-assembly, interpretation

Java rules that are followed:
- `int / int` is integer division, anything involving a `double` is floating-point.
- `int` widens to `double` automatically; assigning a `double` to an `int` is a semantic error (lossy conversion).
- `println` prints doubles Java-style (`10.0`, `3.5`).

Not supported: `import`, strings, `if`/loops, methods other than `main`, arrays, objects, bytecode or machine-code generation. Do not present it as a full Java compiler.
`out`, `println`, `System`, `String` and `main` are reserved words in this mini-language.

## Project layout
| File | Purpose |
| --- | --- |
| `lexer.flex` | JFlex reference grammar (not compiled; runtime lexer is `Lexer.java`) |
| `Lexify.g4` | ANTLR reference grammar (not compiled; runtime parser is `Parser.java`) |
| `Token.java`, `TokenType.java`, `Lexer.java` | Tokenizer |
| `Parser.java`, `Ast.java` | Recursive-descent parser producing the AST (`Expr`, `Stmt`, `Program`) |
| `SymbolTable.java` | Variable table (type + value) |
| `SemanticAnalyzer.java` | Undeclared / duplicate variable and type checks |
| `TacGenerator.java`, `CodeGenerator.java` | Three-address code generation |
| `AssemblyGenerator.java` | Pseudo-assembly output (`DECL`, `MOV`, `NEG`, `ADD`, `SUB`, `MUL`, `DIV`, `PRINT`, `RET`) |
| `Interpreter.java` | Executes the program |
| `Main.java` | Command-line driver |
| `input.java.txt` | Sample program |

## Build
Requires JDK 17+ (`javac` and `java` on PATH).

From the `Java_Compiler` directory:

```bash
make
```

or manually:

```bash
javac *.java
```

## Run

```bash
java Main input.java.txt
java Main input.java.txt --tokens
java Main input.java.txt --ast
java Main input.java.txt --tac
java Main input.java.txt --asm
```

Windows PowerShell: `javac *.java` then `java Main input.java.txt`, or double-click `run.bat`.
Expected sample output (`input.java.txt`): `20` then `8` on separate lines.

`--asm` outputs **pseudo-assembly only**, not runnable CPU instructions or JVM bytecode.
