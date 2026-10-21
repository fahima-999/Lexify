# Lexify — Mini Java-like Compiler / Interpreter

This is a **working educational compiler front-end + interpreter**, NOT a complete Java compiler.

## Requirements
JDK 17+ (`javac` and `java` on PATH).

## Windows PowerShell / VS Code Terminal
```powershell
cd Lexify\Java_Compiler
javac *.java
java Main input.java.txt
```
Or double-click `run.bat` on Windows with a configured JDK.

Expected output:
```
20
8
```

## Inspect compiler stages
```powershell
java Main input.java.txt --tokens
java Main input.java.txt --ast
java Main input.java.txt --tac
```

## Supported mini language
- `int x = 2;` and `int x;`
- `x = x + 1;`
- `print(x);`
- Integer arithmetic `+ - * /`, unary `-`, parentheses
- `//` comments
- Semantic errors for undeclared or duplicate variables

**Not supported:** real Java syntax (`class`, `public static void main`, methods, objects, loops), bytecode, machine code, or JVM compilation. The `.java` files implement the compiler; `input.java.txt` is a mini-language input example.

## Files
`Lexer`, `Token`, `TokenType`: tokenization; `Parser`, `Ast`: parsing/AST; `SemanticAnalyzer`, `SymbolTable`: static checks/storage; `Interpreter`: execution; `TacGenerator`, `CodeGenerator`: three-address intermediate code; `Main`: CLI entry point.

