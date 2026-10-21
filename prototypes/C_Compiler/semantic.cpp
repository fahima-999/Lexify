#include "semantic.h"
#include "symbol_table.h"
#include <set>

namespace {

// Folds an integer constant expression. Returns false if `e` is not constant.
bool constEval(const Expr& e, long long& out) {
    switch (e.kind) {
        case ExprKind::IntLit:
        case ExprKind::CharLit:
            out = e.ival;
            return true;
        case ExprKind::Unary: {
            long long v;
            if (!constEval(*e.args[0], v)) return false;
            switch (e.opc) {
                case Op::Neg:   out = -v; return true;
                case Op::Plus:  out = v; return true;
                case Op::Not:   out = !v; return true;
                case Op::BitNot: out = ~v; return true;
                default: return false;
            }
        }
        case ExprKind::Binary: {
            long long a, b;
            if (!constEval(*e.args[0], a) || !constEval(*e.args[1], b)) return false;
            switch (e.opc) {
                case Op::Add: out = a + b; return true;
                case Op::Sub: out = a - b; return true;
                case Op::Mul: out = a * b; return true;
                case Op::Div: if (b == 0) return false; out = a / b; return true;
                case Op::Mod: if (b == 0) return false; out = a % b; return true;
                case Op::Lt:  out = a < b; return true;
                case Op::Le:  out = a <= b; return true;
                case Op::Gt:  out = a > b; return true;
                case Op::Ge:  out = a >= b; return true;
                case Op::Eq:  out = a == b; return true;
                case Op::Ne:  out = a != b; return true;
                case Op::BitAnd: out = a & b; return true;
                case Op::BitOr:  out = a | b; return true;
                case Op::BitXor: out = a ^ b; return true;
                default: return false;
            }
        }
        default:
            return false;
    }
}

class Analyzer {
    Program& prog;
    SymbolTable syms;
    int loopDepth = 0;
    int switchDepth = 0;

    static void needIntegral(const Expr& e, Type t, const char* op) {
        if (!isIntegral(t))
            semanticError(e.line, std::string("operator '") + op + "' needs integer operands, but got " + typeName(t));
    }

    Type visit(Expr& e) {
        switch (e.kind) {
            case ExprKind::IntLit:
            case ExprKind::FloatLit:
            case ExprKind::CharLit:
                return e.type;

            case ExprKind::Var: {
                int slot = syms.lookup(e.name);
                if (slot < 0) semanticError(e.line, "undeclared variable '" + e.name + "'");
                e.slot = slot;
                e.type = syms.symbol(slot).type;
                return e.type;
            }

            case ExprKind::Unary: {
                Type t = visit(*e.args[0]);
                if (e.opc == Op::Not) e.type = Type::Int;
                else if (e.opc == Op::BitNot) { needIntegral(e, t, "~"); e.type = promote(t); }
                else e.type = promote(t);
                return e.type;
            }

            case ExprKind::Binary: {
                Type a = visit(*e.args[0]);
                Type b = visit(*e.args[1]);
                switch (e.opc) {
                    case Op::Lt: case Op::Le: case Op::Gt: case Op::Ge: case Op::Eq: case Op::Ne:
                        e.type = Type::Int;
                        break;
                    case Op::Shl: case Op::Shr:
                        needIntegral(e, a, e.op.c_str());
                        needIntegral(e, b, e.op.c_str());
                        e.type = promote(a);
                        break;
                    case Op::Mod: case Op::BitAnd: case Op::BitOr: case Op::BitXor:
                        needIntegral(e, a, e.op.c_str());
                        needIntegral(e, b, e.op.c_str());
                        e.type = commonType(a, b);
                        break;
                    default:
                        e.type = commonType(a, b);
                }
                if (e.opc == Op::Div || e.opc == Op::Mod) {
                    long long v;
                    if (isIntegral(b) && constEval(*e.args[1], v) && v == 0)
                        semanticError(e.line, e.opc == Op::Div ? "division by zero" : "modulo by zero");
                }
                return e.type;
            }

            case ExprKind::Logical:
                visit(*e.args[0]);
                visit(*e.args[1]);
                e.type = Type::Int;
                return e.type;

            case ExprKind::Assign: {
                Type l = visit(*e.args[0]);
                Type r = visit(*e.args[1]);
                switch (e.opc) {
                    case Op::Mod: case Op::BitAnd: case Op::BitOr: case Op::BitXor: case Op::Shl: case Op::Shr:
                        needIntegral(e, l, e.op.c_str());
                        needIntegral(e, r, e.op.c_str());
                        break;
                    case Op::Div: {
                        long long v;
                        if (isIntegral(r) && constEval(*e.args[1], v) && v == 0) semanticError(e.line, "division by zero");
                        break;
                    }
                    default: break;
                }
                e.type = l;
                return e.type;
            }

            case ExprKind::IncDec:
                e.type = visit(*e.args[0]);
                return e.type;

            case ExprKind::Ternary: {
                visit(*e.args[0]);
                Type a = visit(*e.args[1]);
                Type b = visit(*e.args[2]);
                e.type = commonType(a, b);
                return e.type;
            }

            case ExprKind::Cast:
                visit(*e.args[0]);
                return e.type;  // target type was set by the parser

            case ExprKind::Comma:
                visit(*e.args[0]);
                e.type = visit(*e.args[1]);
                return e.type;
        }
        return Type::Int;
    }

    void checkPrintf(Stmt& s) {
        auto pieces = parseFormat(s.text, s.line);
        std::vector<const FormatPiece*> specs;
        for (auto& p : pieces)
            if (p.isSpec) specs.push_back(&p);
        for (auto& a : s.args) visit(*a);
        if (specs.size() != s.args.size())
            semanticError(s.line, "printf format has " + std::to_string(specs.size()) + " conversion(s) but " +
                                      std::to_string(s.args.size()) + " argument(s) were given");
        for (size_t i = 0; i < specs.size(); ++i) {
            char c = specs[i]->conv;
            Type t = s.args[i]->type;
            bool wantFloat = std::string("feEgG").find(c) != std::string::npos;
            if (wantFloat && !isFloating(t))
                semanticError(s.line, std::string("printf: '%") + c + "' expects a float/double argument but argument " +
                                          std::to_string(i + 1) + " is " + typeName(t));
            if (!wantFloat && !isIntegral(t))
                semanticError(s.line, std::string("printf: '%") + c + "' expects an integer argument but argument " +
                                          std::to_string(i + 1) + " is " + typeName(t));
        }
    }

    void checkScanf(Stmt& s) {
        auto pieces = parseFormat(s.text, s.line);
        std::vector<const FormatPiece*> specs;
        for (auto& p : pieces)
            if (p.isSpec) specs.push_back(&p);
        for (auto& a : s.args) visit(*a);
        if (specs.size() != s.args.size())
            semanticError(s.line, "scanf format has " + std::to_string(specs.size()) + " conversion(s) but " +
                                      std::to_string(s.args.size()) + " variable(s) were given");
        for (size_t i = 0; i < specs.size(); ++i) {
            const FormatPiece& p = *specs[i];
            Type t = s.args[i]->type;
            Type want;
            if (p.conv == 'd' || p.conv == 'i') want = p.longs > 0 ? Type::Long : Type::Int;
            else if (p.conv == 'f' || p.conv == 'e' || p.conv == 'g') want = p.longs > 0 ? Type::Double : Type::Float;
            else if (p.conv == 'c') want = Type::Char;
            else semanticError(s.line, std::string("scanf supports only %d, %ld, %f, %lf and %c, not '%") + p.conv + "'");
            if (t != want)
                semanticError(s.line, "scanf: format expects a variable of type " + std::string(typeName(want)) +
                                          " but '" + s.args[i]->name + "' is " + typeName(t));
        }
    }

    void visitDecl(Stmt& s) {
        for (auto& d : s.decls) {
            if (d.init) visit(*d.init);  // the initializer cannot use the variable being declared
            d.slot = syms.declare(d.name, s.declType, d.line);
        }
    }

    void visitSwitch(Stmt& s) {
        Type t = visit(*s.cond);
        if (!isIntegral(t)) semanticError(s.line, std::string("switch needs an integer expression, but got ") + typeName(t));
        std::set<long long> seen;
        bool hasDefault = false;
        ++switchDepth;
        syms.enterScope();
        for (auto& c : s.cases) {
            if (c.isDefault) {
                if (hasDefault) semanticError(c.line, "duplicate 'default' label in switch");
                hasDefault = true;
            } else {
                visit(*c.valueExpr);
                if (!constEval(*c.valueExpr, c.value))
                    semanticError(c.line, "case label must be a constant integer expression");
                if (!seen.insert(c.value).second)
                    semanticError(c.line, "duplicate case value " + std::to_string(c.value));
            }
            for (auto& st : c.body) visit(*st);
        }
        syms.exitScope();
        --switchDepth;
    }

    void visit(Stmt& s) {
        switch (s.kind) {
            case StmtKind::Empty: break;
            case StmtKind::Decl: visitDecl(s); break;
            case StmtKind::ExprStmt: visit(*s.expr); break;
            case StmtKind::Print: visit(*s.args[0]); break;
            case StmtKind::Printf: checkPrintf(s); break;
            case StmtKind::Scanf: checkScanf(s); break;
            case StmtKind::Block:
                syms.enterScope();
                for (auto& c : s.stmts) visit(*c);
                syms.exitScope();
                break;
            case StmtKind::If:
                visit(*s.cond);
                visit(*s.thenS);
                if (s.elseS) visit(*s.elseS);
                break;
            case StmtKind::While:
            case StmtKind::DoWhile:
                visit(*s.cond);
                ++loopDepth;
                visit(*s.thenS);
                --loopDepth;
                break;
            case StmtKind::For:
                syms.enterScope();  // the loop variable lives only inside the for
                if (s.init) visit(*s.init);
                if (s.cond) visit(*s.cond);
                if (s.step) visit(*s.step);
                ++loopDepth;
                visit(*s.thenS);
                --loopDepth;
                syms.exitScope();
                break;
            case StmtKind::Switch: visitSwitch(s); break;
            case StmtKind::Break:
                if (loopDepth == 0 && switchDepth == 0) semanticError(s.line, "'break' outside of a loop or switch");
                break;
            case StmtKind::Continue:
                if (loopDepth == 0) semanticError(s.line, "'continue' outside of a loop");
                break;
            case StmtKind::Return:
                if (s.expr) visit(*s.expr);
                break;
        }
    }

    void visit(const ExprPtr& e) { visit(*e); }
    void visit(const StmtPtr& s) { visit(*s); }

public:
    explicit Analyzer(Program& p) : prog(p) {}

    void run() {
        visit(*prog.body);
        for (auto& s : syms.all()) {
            prog.varNames.push_back(s.uniqueName);
            prog.varTypes.push_back(s.type);
        }
    }
};

}  // namespace

void checkSemantics(Program& program) { Analyzer(program).run(); }
