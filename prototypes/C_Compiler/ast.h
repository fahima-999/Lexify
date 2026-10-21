#pragma once
// ast.h - tokens, types, AST nodes and shared helpers for the Lexify C compiler.
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

// ------------------------------------------------------------------ Tokens
struct Token {
    std::string kind;   // "ID", "INT_LIT", "STRING", "EOF", a keyword ("if") or an operator ("+=")
    std::string text;   // the lexeme (for literals: the decoded value)
    int line = 0;
};

// ------------------------------------------------------------------- Types
enum class Type { Void, Char, Int, Long, Float, Double };

const char* typeName(Type t);
bool isFloating(Type t);
bool isIntegral(Type t);
Type promote(Type t);                // char -> int, others unchanged
Type commonType(Type a, Type b);     // usual arithmetic conversions

// ------------------------------------------------------------------ Errors
struct CompilerError : std::runtime_error {
    using std::runtime_error::runtime_error;
};
[[noreturn]] void lexError(int line, const std::string& msg);
[[noreturn]] void syntaxError(int line, const std::string& msg);
[[noreturn]] void semanticError(int line, const std::string& msg);
[[noreturn]] void runtimeError(int line, const std::string& msg);

// --------------------------------------------------------------- Operators
enum class Op {
    None,
    Add, Sub, Mul, Div, Mod,
    Shl, Shr,
    Lt, Le, Gt, Ge, Eq, Ne,
    BitAnd, BitOr, BitXor,
    LogAnd, LogOr,
    Neg, Plus, Not, BitNot
};
Op binaryOpFromText(const std::string& text);   // "+" -> Add, "<=" -> Le ...

// ------------------------------------------------------------- Expressions
enum class ExprKind {
    IntLit, FloatLit, CharLit,
    Var,
    Unary,      // + - ! ~
    Binary,     // arithmetic, bitwise, relational
    Logical,    // && ||  (short circuit)
    Assign,     // = += -= *= /= %= &= |= ^= <<= >>=
    IncDec,     // ++x  x++  --x  x--
    Ternary,    // c ? a : b
    Cast,       // (type) expr
    Comma       // a , b
};

struct Expr;
using ExprPtr = std::shared_ptr<Expr>;

struct Expr {
    ExprKind kind = ExprKind::IntLit;
    int line = 0;
    std::string op;          // operator text as written
    Op opc = Op::None;       // decoded operator (Assign: the arithmetic part, None for plain '=')
    std::string name;        // Var: identifier
    long long ival = 0;      // IntLit / CharLit
    double dval = 0.0;       // FloatLit
    bool prefix = false;     // IncDec: true for ++x
    Type type = Type::Int;   // result type (filled in by semantic analysis; literals/casts set it directly)
    int slot = -1;           // Var: variable slot (filled in by semantic analysis)
    std::vector<ExprPtr> args;
};

ExprPtr makeExpr(ExprKind kind, int line);

// -------------------------------------------------------------- Statements
enum class StmtKind {
    Empty, Decl, ExprStmt, Print, Printf, Scanf,
    Block, If, While, DoWhile, For, Switch,
    Break, Continue, Return
};

struct Stmt;
using StmtPtr = std::shared_ptr<Stmt>;

struct Declarator {
    std::string name;
    ExprPtr init;            // may be null
    int slot = -1;           // filled in by semantic analysis
    int line = 0;
};

struct SwitchCase {
    bool isDefault = false;
    ExprPtr valueExpr;       // case label expression
    long long value = 0;     // folded by semantic analysis
    std::vector<StmtPtr> body;
    int line = 0;
};

struct Stmt {
    StmtKind kind = StmtKind::Empty;
    int line = 0;

    Type declType = Type::Int;            // Decl
    std::vector<Declarator> decls;        // Decl

    ExprPtr expr;                         // ExprStmt expression, Return value (optional)
    ExprPtr cond;                         // If / While / DoWhile / For (optional) / Switch
    ExprPtr step;                         // For: step expression (optional)
    StmtPtr init;                         // For: init statement (optional)
    StmtPtr thenS;                        // If: then-branch.  While/DoWhile/For: loop body
    StmtPtr elseS;                        // If: else-branch (optional)
    std::vector<StmtPtr> stmts;           // Block
    std::string text;                     // Printf / Scanf: format string
    std::vector<ExprPtr> args;            // Print / Printf arguments, Scanf target variables
    std::vector<SwitchCase> cases;        // Switch
};

StmtPtr makeStmt(StmtKind kind, int line);

struct Program {
    StmtPtr body;                         // one big Block
    bool hasMain = false;
    std::vector<std::string> varNames;    // indexed by variable slot (unique names)
    std::vector<Type> varTypes;           // indexed by variable slot
};

// -------------------------------------------------- printf / scanf formats
struct FormatPiece {
    bool isSpec = false;     // false: literal text, true: a conversion like %5.2f
    std::string text;        // literal text, or the spec without length modifiers ("%5.2")
    char conv = 0;           // conversion character: d i u x X o c f e E g G
    int longs = 0;           // number of 'l' modifiers
};
std::vector<FormatPiece> parseFormat(const std::string& fmt, int line);

// --------------------------------------------------------------- Utilities
std::string escapeText(const std::string& s);      // "a\n" -> a\\n (for display)
std::string exprToString(const Expr& e);           // s-expression style
std::string dumpProgram(const Program& program);   // pretty AST (for --ast)
