#include "ast.h"
#include <cctype>
#include <sstream>

// ------------------------------------------------------------------- Types
const char* typeName(Type t) {
    switch (t) {
        case Type::Void:   return "void";
        case Type::Char:   return "char";
        case Type::Int:    return "int";
        case Type::Long:   return "long";
        case Type::Float:  return "float";
        case Type::Double: return "double";
    }
    return "?";
}
bool isFloating(Type t) { return t == Type::Float || t == Type::Double; }
bool isIntegral(Type t) { return t == Type::Char || t == Type::Int || t == Type::Long; }
Type promote(Type t) { return t == Type::Char ? Type::Int : t; }
Type commonType(Type a, Type b) {
    if (a == Type::Double || b == Type::Double) return Type::Double;
    if (a == Type::Float || b == Type::Float) return Type::Float;
    if (a == Type::Long || b == Type::Long) return Type::Long;
    return Type::Int;
}

// ------------------------------------------------------------------ Errors
static std::string withLine(const char* phase, int line, const std::string& msg) {
    std::string s = phase;
    if (line > 0) s += " (line " + std::to_string(line) + ")";
    return s + ": " + msg;
}
void lexError(int line, const std::string& msg)      { throw CompilerError(withLine("Lexical error", line, msg)); }
void syntaxError(int line, const std::string& msg)   { throw CompilerError(withLine("Syntax error", line, msg)); }
void semanticError(int line, const std::string& msg) { throw CompilerError(withLine("Semantic error", line, msg)); }
void runtimeError(int line, const std::string& msg)  { throw CompilerError(withLine("Runtime error", line, msg)); }

// --------------------------------------------------------------- Operators
Op binaryOpFromText(const std::string& t) {
    if (t == "+")  return Op::Add;
    if (t == "-")  return Op::Sub;
    if (t == "*")  return Op::Mul;
    if (t == "/")  return Op::Div;
    if (t == "%")  return Op::Mod;
    if (t == "<<") return Op::Shl;
    if (t == ">>") return Op::Shr;
    if (t == "<")  return Op::Lt;
    if (t == "<=") return Op::Le;
    if (t == ">")  return Op::Gt;
    if (t == ">=") return Op::Ge;
    if (t == "==") return Op::Eq;
    if (t == "!=") return Op::Ne;
    if (t == "&")  return Op::BitAnd;
    if (t == "|")  return Op::BitOr;
    if (t == "^")  return Op::BitXor;
    if (t == "&&") return Op::LogAnd;
    if (t == "||") return Op::LogOr;
    return Op::None;
}

// ------------------------------------------------------------ Node factories
ExprPtr makeExpr(ExprKind kind, int line) {
    auto e = std::make_shared<Expr>();
    e->kind = kind;
    e->line = line;
    return e;
}
StmtPtr makeStmt(StmtKind kind, int line) {
    auto s = std::make_shared<Stmt>();
    s->kind = kind;
    s->line = line;
    return s;
}

// -------------------------------------------------- printf / scanf formats
std::vector<FormatPiece> parseFormat(const std::string& f, int line) {
    std::vector<FormatPiece> out;
    std::string lit;
    auto flush = [&]() {
        if (!lit.empty()) {
            FormatPiece p;
            p.text = lit;
            out.push_back(p);
            lit.clear();
        }
    };
    size_t i = 0;
    while (i < f.size()) {
        if (f[i] != '%') { lit += f[i++]; continue; }
        if (i + 1 < f.size() && f[i + 1] == '%') { lit += '%'; i += 2; continue; }
        flush();
        size_t j = i + 1;
        std::string spec = "%";
        while (j < f.size() && std::string("-+ 0#").find(f[j]) != std::string::npos) spec += f[j++];
        while (j < f.size() && std::isdigit((unsigned char)f[j])) spec += f[j++];
        if (j < f.size() && f[j] == '.') {
            spec += f[j++];
            while (j < f.size() && std::isdigit((unsigned char)f[j])) spec += f[j++];
        }
        int longs = 0;
        while (j < f.size() && (f[j] == 'l' || f[j] == 'h')) {
            if (f[j] == 'l') ++longs;
            ++j;
        }
        if (j >= f.size()) semanticError(line, "incomplete conversion specifier in format string");
        char c = f[j];
        if (std::string("diuxXocfeEgG").find(c) == std::string::npos) {
            if (c == 's') semanticError(line, "'%s' is not supported (this compiler has no strings)");
            semanticError(line, std::string("unsupported conversion '%") + c + "' in format string");
        }
        FormatPiece p;
        p.isSpec = true;
        p.text = spec;
        p.conv = c;
        p.longs = longs;
        out.push_back(p);
        i = j + 1;
    }
    flush();
    return out;
}

// --------------------------------------------------------------- Utilities
std::string escapeText(const std::string& s) {
    std::string r;
    for (char c : s) {
        switch (c) {
            case '\n': r += "\\n"; break;
            case '\t': r += "\\t"; break;
            case '\r': r += "\\r"; break;
            case '\0': r += "\\0"; break;
            case '\\': r += "\\\\"; break;
            case '"':  r += "\\\""; break;
            default:   r += c;
        }
    }
    return r;
}

std::string exprToString(const Expr& e) {
    switch (e.kind) {
        case ExprKind::IntLit:
        case ExprKind::CharLit: return std::to_string(e.ival);
        case ExprKind::FloatLit: { std::ostringstream o; o << e.dval; return o.str(); }
        case ExprKind::Var: return e.name;
        case ExprKind::Unary: return "(" + e.op + " " + exprToString(*e.args[0]) + ")";
        case ExprKind::Binary:
        case ExprKind::Logical:
        case ExprKind::Assign:
        case ExprKind::Comma:
            return "(" + e.op + " " + exprToString(*e.args[0]) + " " + exprToString(*e.args[1]) + ")";
        case ExprKind::IncDec:
            return e.prefix ? "(" + e.op + e.args[0]->name + ")" : "(" + e.args[0]->name + e.op + ")";
        case ExprKind::Ternary:
            return "(?: " + exprToString(*e.args[0]) + " " + exprToString(*e.args[1]) + " " +
                   exprToString(*e.args[2]) + ")";
        case ExprKind::Cast:
            return std::string("((") + typeName(e.type) + ") " + exprToString(*e.args[0]) + ")";
    }
    return "?";
}

static void dumpStmt(const Stmt& s, int ind, std::ostringstream& o) {
    std::string pad(ind * 2, ' ');
    switch (s.kind) {
        case StmtKind::Empty: o << pad << "Empty\n"; break;
        case StmtKind::Decl:
            o << pad << "Decl " << typeName(s.declType) << ":";
            for (size_t i = 0; i < s.decls.size(); ++i) {
                o << (i ? ", " : " ") << s.decls[i].name;
                if (s.decls[i].init) o << " = " << exprToString(*s.decls[i].init);
            }
            o << "\n";
            break;
        case StmtKind::ExprStmt: o << pad << "Expr " << exprToString(*s.expr) << "\n"; break;
        case StmtKind::Print:    o << pad << "Print " << exprToString(*s.args[0]) << "\n"; break;
        case StmtKind::Printf:
        case StmtKind::Scanf:
            o << pad << (s.kind == StmtKind::Printf ? "Printf" : "Scanf") << " \"" << escapeText(s.text) << "\"";
            for (auto& a : s.args) o << ", " << (s.kind == StmtKind::Scanf ? "&" : "") << exprToString(*a);
            o << "\n";
            break;
        case StmtKind::Block:
            o << pad << "Block\n";
            for (auto& c : s.stmts) dumpStmt(*c, ind + 1, o);
            break;
        case StmtKind::If:
            o << pad << "If " << exprToString(*s.cond) << "\n";
            o << pad << "  Then:\n";
            dumpStmt(*s.thenS, ind + 2, o);
            if (s.elseS) {
                o << pad << "  Else:\n";
                dumpStmt(*s.elseS, ind + 2, o);
            }
            break;
        case StmtKind::While:
            o << pad << "While " << exprToString(*s.cond) << "\n";
            dumpStmt(*s.thenS, ind + 1, o);
            break;
        case StmtKind::DoWhile:
            o << pad << "DoWhile " << exprToString(*s.cond) << "\n";
            dumpStmt(*s.thenS, ind + 1, o);
            break;
        case StmtKind::For:
            o << pad << "For\n";
            o << pad << "  Init:\n";
            if (s.init) dumpStmt(*s.init, ind + 2, o);
            o << pad << "  Cond: " << (s.cond ? exprToString(*s.cond) : "(none)") << "\n";
            o << pad << "  Step: " << (s.step ? exprToString(*s.step) : "(none)") << "\n";
            o << pad << "  Body:\n";
            dumpStmt(*s.thenS, ind + 2, o);
            break;
        case StmtKind::Switch:
            o << pad << "Switch " << exprToString(*s.cond) << "\n";
            for (auto& c : s.cases) {
                o << pad << "  " << (c.isDefault ? std::string("default") : "case " + exprToString(*c.valueExpr)) << ":\n";
                for (auto& st : c.body) dumpStmt(*st, ind + 2, o);
            }
            break;
        case StmtKind::Break:    o << pad << "Break\n"; break;
        case StmtKind::Continue: o << pad << "Continue\n"; break;
        case StmtKind::Return:
            o << pad << "Return" << (s.expr ? " " + exprToString(*s.expr) : "") << "\n";
            break;
    }
}

std::string dumpProgram(const Program& program) {
    std::ostringstream o;
    o << "Program" << (program.hasMain ? " (with main)" : "") << "\n";
    if (program.body) dumpStmt(*program.body, 1, o);
    return o.str();
}
