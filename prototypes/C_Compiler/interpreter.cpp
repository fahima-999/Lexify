#include "interpreter.h"
#include <climits>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <iostream>

namespace {

// A runtime value. Integral types use `i`, float/double use `d`.
struct Value {
    Type type = Type::Int;
    long long i = 0;
    double d = 0.0;
};

const long long kMaxLoopIterations = 100000000LL;  // guard against endless loops

long long doubleToLL(double d) {
    if (d != d) return 0;
    if (d >= 9.2233720368547758e18) return LLONG_MAX;
    if (d <= -9.2233720368547758e18) return LLONG_MIN;
    return (long long)d;
}

double toDouble(const Value& v) { return isFloating(v.type) ? v.d : (double)v.i; }

// Converts a value to type `t` (this is what assignments, casts and arithmetic conversions do).
Value convert(const Value& v, Type t) {
    Value r;
    r.type = t;
    if (isFloating(t)) {
        double x = toDouble(v);
        r.d = (t == Type::Float) ? (double)(float)x : x;
    } else {
        long long x = isFloating(v.type) ? doubleToLL(v.d) : v.i;
        switch (t) {
            case Type::Char: r.i = (int8_t)x; break;
            case Type::Int:  r.i = (int32_t)x; break;
            default:         r.i = x; break;
        }
    }
    return r;
}

Value makeInt(Type t, long long v) {
    Value r;
    r.type = Type::Long;
    r.i = v;
    return convert(r, t);
}
Value makeDouble(Type t, double v) {
    Value r;
    r.type = Type::Double;
    r.d = v;
    return convert(r, t);
}

bool truth(const Value& v) { return isFloating(v.type) ? v.d != 0.0 : v.i != 0; }

template <class T>
bool compare(Op op, T x, T y) {
    switch (op) {
        case Op::Lt: return x < y;
        case Op::Le: return x <= y;
        case Op::Gt: return x > y;
        case Op::Ge: return x >= y;
        case Op::Eq: return x == y;
        default:     return x != y;
    }
}

long long wrapAdd(long long a, long long b) { return (long long)((unsigned long long)a + (unsigned long long)b); }
long long wrapSub(long long a, long long b) { return (long long)((unsigned long long)a - (unsigned long long)b); }
long long wrapMul(long long a, long long b) { return (long long)((unsigned long long)a * (unsigned long long)b); }

Value binop(Op op, const Value& a, const Value& b, int line) {
    switch (op) {
        case Op::Lt: case Op::Le: case Op::Gt: case Op::Ge: case Op::Eq: case Op::Ne: {
            bool r = (isFloating(a.type) || isFloating(b.type)) ? compare(op, toDouble(a), toDouble(b))
                                                                  : compare(op, a.i, b.i);
            return makeInt(Type::Int, r ? 1 : 0);
        }
        case Op::Shl: case Op::Shr: {
            Type t = promote(a.type);
            int bits = (t == Type::Long) ? 64 : 32;
            if (b.i < 0 || b.i >= bits) runtimeError(line, "invalid shift count " + std::to_string(b.i));
            long long r = (op == Op::Shl) ? (long long)((unsigned long long)a.i << b.i) : (a.i >> b.i);
            return makeInt(t, r);
        }
        default: break;
    }

    Type t = commonType(a.type, b.type);
    if (isFloating(t)) {
        double x = toDouble(a), y = toDouble(b), r = 0;
        switch (op) {
            case Op::Add: r = x + y; break;
            case Op::Sub: r = x - y; break;
            case Op::Mul: r = x * y; break;
            case Op::Div: r = x / y; break;  // IEEE: x/0.0 gives inf / nan like in C
            default: runtimeError(line, "invalid operator for floating-point operands");
        }
        return makeDouble(t, r);
    }

    long long x = a.i, y = b.i, r = 0;
    switch (op) {
        case Op::Add: r = wrapAdd(x, y); break;
        case Op::Sub: r = wrapSub(x, y); break;
        case Op::Mul: r = wrapMul(x, y); break;
        case Op::Div:
            if (y == 0) runtimeError(line, "division by zero");
            r = (y == -1) ? wrapSub(0, x) : x / y;
            break;
        case Op::Mod:
            if (y == 0) runtimeError(line, "modulo by zero");
            r = (y == -1) ? 0 : x % y;
            break;
        case Op::BitAnd: r = x & y; break;
        case Op::BitOr:  r = x | y; break;
        case Op::BitXor: r = x ^ y; break;
        default: runtimeError(line, "unknown operator");
    }
    return makeInt(t, r);
}

template <class T>
std::string formatWith(const std::string& spec, T value) {
    int n = std::snprintf(nullptr, 0, spec.c_str(), value);
    if (n < 0) return "";
    std::string buf((size_t)n + 1, '\0');
    std::snprintf(&buf[0], buf.size(), spec.c_str(), value);
    buf.resize((size_t)n);
    return buf;
}

std::string formatOne(const FormatPiece& p, const Value& v) {
    switch (p.conv) {
        case 'd':
        case 'i': return formatWith(p.text + "lld", (long long)v.i);
        case 'u':
        case 'x':
        case 'X':
        case 'o': {
            unsigned long long u = (v.type == Type::Long) ? (unsigned long long)v.i : (unsigned long long)(uint32_t)v.i;
            return formatWith(p.text + "ll" + p.conv, u);
        }
        case 'c': return formatWith(p.text + "c", (int)v.i);
        default:  return formatWith(p.text + p.conv, toDouble(v));
    }
}

enum class Flow { Normal, Break, Continue, Return };

class Interpreter {
    const Program& prog;
    std::vector<Value> mem;
    long long iterations = 0;
    int exitCode = 0;

    void tick(int line) {
        if (++iterations > kMaxLoopIterations)
            runtimeError(line, "loop limit exceeded - the program probably contains an infinite loop");
    }

    Value zero(Type t) {
        Value v;
        v.type = t;
        return v;
    }

    Value eval(const Expr& e) {
        switch (e.kind) {
            case ExprKind::IntLit:
            case ExprKind::CharLit: return makeInt(e.type, e.ival);
            case ExprKind::FloatLit: return makeDouble(e.type, e.dval);
            case ExprKind::Var: return mem[e.slot];

            case ExprKind::Unary: {
                Value v = eval(*e.args[0]);
                switch (e.opc) {
                    case Op::Not: return makeInt(Type::Int, truth(v) ? 0 : 1);
                    case Op::BitNot: return makeInt(promote(v.type), ~v.i);
                    case Op::Plus: return convert(v, promote(v.type));
                    default:  // Neg
                        if (isFloating(v.type)) return makeDouble(v.type, -v.d);
                        return makeInt(promote(v.type), wrapSub(0, v.i));
                }
            }

            case ExprKind::Binary: {
                Value a = eval(*e.args[0]);
                Value b = eval(*e.args[1]);
                return binop(e.opc, a, b, e.line);
            }

            case ExprKind::Logical: {
                bool l = truth(eval(*e.args[0]));
                if (e.opc == Op::LogAnd) {
                    if (!l) return makeInt(Type::Int, 0);
                } else if (l) {
                    return makeInt(Type::Int, 1);
                }
                return makeInt(Type::Int, truth(eval(*e.args[1])) ? 1 : 0);
            }

            case ExprKind::Assign: {
                const Expr& target = *e.args[0];
                Value rhs = eval(*e.args[1]);
                Value nv;
                if (e.opc == Op::None) nv = convert(rhs, target.type);
                else nv = convert(binop(e.opc, mem[target.slot], rhs, e.line), target.type);
                mem[target.slot] = nv;
                return nv;
            }

            case ExprKind::IncDec: {
                const Expr& target = *e.args[0];
                Value old = mem[target.slot];
                Value nv = convert(binop(e.opc, old, makeInt(Type::Int, 1), e.line), target.type);
                mem[target.slot] = nv;
                return e.prefix ? nv : old;
            }

            case ExprKind::Ternary: {
                bool c = truth(eval(*e.args[0]));
                return convert(eval(*e.args[c ? 1 : 2]), e.type);
            }

            case ExprKind::Cast: return convert(eval(*e.args[0]), e.type);

            case ExprKind::Comma:
                eval(*e.args[0]);
                return eval(*e.args[1]);
        }
        return Value();
    }

    void doPrint(const Value& v) {
        if (isFloating(v.type)) std::cout << formatWith("%g", v.d) << '\n';
        else std::cout << v.i << '\n';
    }

    void doPrintf(const Stmt& s) {
        auto pieces = parseFormat(s.text, s.line);
        size_t arg = 0;
        for (auto& p : pieces) {
            if (!p.isSpec) std::cout << p.text;
            else std::cout << formatOne(p, eval(*s.args[arg++]));
        }
    }

    void skipSpaces() {
        while (std::cin && std::isspace(std::cin.peek())) std::cin.get();
    }

    void doScanf(const Stmt& s) {
        auto pieces = parseFormat(s.text, s.line);
        size_t arg = 0;
        for (auto& p : pieces) {
            if (!p.isSpec) {
                for (char ch : p.text) {
                    if (std::isspace((unsigned char)ch)) skipSpaces();
                    else if (std::cin.peek() == (unsigned char)ch) std::cin.get();
                    else return;
                }
                continue;
            }
            const Expr& target = *s.args[arg++];
            Value v;
            if (p.conv == 'c') {
                int ch = std::cin.get();
                if (ch == EOF) return;
                v = makeInt(Type::Char, ch);
            } else if (p.conv == 'd' || p.conv == 'i') {
                long long x;
                skipSpaces();
                if (!(std::cin >> x)) return;
                v = makeInt(Type::Long, x);
            } else {
                double x;
                skipSpaces();
                if (!(std::cin >> x)) return;
                v = makeDouble(Type::Double, x);
            }
            mem[target.slot] = convert(v, target.type);
        }
    }

    Flow execBlockList(const std::vector<StmtPtr>& list) {
        for (auto& st : list) {
            Flow f = exec(*st);
            if (f != Flow::Normal) return f;
        }
        return Flow::Normal;
    }

    Flow exec(const Stmt& s) {
        switch (s.kind) {
            case StmtKind::Empty: return Flow::Normal;

            case StmtKind::Decl:
                for (auto& d : s.decls) {
                    // a declaration without initializer starts at 0 (C would leave garbage)
                    if (d.init) mem[d.slot] = convert(eval(*d.init), s.declType);
                    else mem[d.slot] = zero(s.declType);
                }
                return Flow::Normal;

            case StmtKind::ExprStmt: eval(*s.expr); return Flow::Normal;
            case StmtKind::Print: doPrint(eval(*s.args[0])); return Flow::Normal;
            case StmtKind::Printf: doPrintf(s); return Flow::Normal;
            case StmtKind::Scanf: doScanf(s); return Flow::Normal;

            case StmtKind::Block: return execBlockList(s.stmts);

            case StmtKind::If:
                if (truth(eval(*s.cond))) return exec(*s.thenS);
                if (s.elseS) return exec(*s.elseS);
                return Flow::Normal;

            case StmtKind::While:
                while (truth(eval(*s.cond))) {
                    tick(s.line);
                    Flow f = exec(*s.thenS);
                    if (f == Flow::Break) break;
                    if (f == Flow::Return) return f;
                }
                return Flow::Normal;

            case StmtKind::DoWhile:
                do {
                    tick(s.line);
                    Flow f = exec(*s.thenS);
                    if (f == Flow::Break) break;
                    if (f == Flow::Return) return f;
                } while (truth(eval(*s.cond)));
                return Flow::Normal;

            case StmtKind::For:
                if (s.init) exec(*s.init);
                while (!s.cond || truth(eval(*s.cond))) {
                    tick(s.line);
                    Flow f = exec(*s.thenS);
                    if (f == Flow::Break) break;
                    if (f == Flow::Return) return f;
                    if (s.step) eval(*s.step);
                }
                return Flow::Normal;

            case StmtKind::Switch: {
                long long v = eval(*s.cond).i;
                int start = -1, def = -1;
                for (size_t k = 0; k < s.cases.size(); ++k) {
                    if (s.cases[k].isDefault) def = (int)k;
                    else if (start < 0 && s.cases[k].value == v) start = (int)k;
                }
                if (start < 0) start = def;
                if (start < 0) return Flow::Normal;
                for (size_t k = (size_t)start; k < s.cases.size(); ++k) {  // fall through like C
                    Flow f = execBlockList(s.cases[k].body);
                    if (f == Flow::Break) return Flow::Normal;
                    if (f != Flow::Normal) return f;
                }
                return Flow::Normal;
            }

            case StmtKind::Break: return Flow::Break;
            case StmtKind::Continue: return Flow::Continue;
            case StmtKind::Return:
                if (s.expr) exitCode = (int)convert(eval(*s.expr), Type::Int).i;
                return Flow::Return;
        }
        return Flow::Normal;
    }

public:
    explicit Interpreter(const Program& p) : prog(p) {
        mem.resize(p.varTypes.size());
        for (size_t k = 0; k < mem.size(); ++k) mem[k] = zero(p.varTypes[k]);
    }

    int run() {
        exec(*prog.body);
        std::cout.flush();
        return exitCode;
    }
};

}  // namespace

int execute(const Program& program) {
    Interpreter in(program);
    try {
        return in.run();
    } catch (...) {
        std::cout.flush();
        throw;
    }
}
