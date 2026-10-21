# Lexify C++ Mini Compiler

A small **educational C++-subset interpreter**, not a complete ISO C++ compiler.

## Supported syntax
- `int main() { ... }`, optional `using namespace std;`
- `int` and `double` declarations, assignments
- Arithmetic `+ - * /`, parentheses, unary minus
- `cout << expression;` and `return expression;`
- `//` and `/* ... */` comments
- Lexing, parsing, undeclared/duplicate-variable checks, three-address code, pseudo-assembly, interpretation

Not supported: `#include`, `std::cout`, strings, loops, functions, arrays, classes, real assembly or machine-code generation. Do not present it as a full C++ compiler.

## Build
Requires `g++` (MinGW-w64 on Windows) or another C++17 compiler.

From the `CPP_Compiler` directory:

```bash
g++ -std=c++17 main.cpp lexer.cpp parser.cpp ast.cpp semantic.cpp symbol_table.cpp interpreter.cpp codegen.cpp assembly.cpp -o lexify_cpp
```

On Windows the output can be named `lexify_cpp.exe` using `-o lexify_cpp.exe`.

## Run

```bash
./lexify_cpp input.cpp
./lexify_cpp input.cpp --tokens
./lexify_cpp input.cpp --tac
./lexify_cpp input.cpp --asm
```

Windows PowerShell: `./lexify_cpp.exe input.cpp` (or ` .\lexify_cpp.exe input.cpp`).
Expected sample output: `20` then `17` on separate lines.

`--asm` outputs **pseudo-assembly only**, not runnable CPU instructions.
