#include "tac.h"
#include <sstream>

namespace {

class TacGen {
    const Program& prog;
    std::vector<TacInstr> out;
    int temps = 0, labels = 0;
    std::vector<std::string> breakStack, continueStack;

    std::string newTemp() { return "t" + std::to_string(++temps); }
    std::string newLabel() { return "L" + std::to_string(++labels); }

    void emit(TacInstr i) { out.push_back(std::move(i)); }
    void label(const std::string& l) { TacInstr i; i.kind = TacKind::Label; i.res = l; emit(i); }
    void jump(const std::string& l) { TacInstr i; i.kind = TacKind::Goto; i.res = l; emit(i); }
    void branch(bool onTrue, const std::string& cond, const std::string& l) {
        TacInstr i;
        i.kind = onTrue ? TacKind::IfTrue : TacKind::IfFalse;
        i.a = cond;
        i.res = l;
        emit(i);
    }
    void assign(const std::string& dst, const std::string& src) {
        TacInstr i; i.kind = TacKind::Assign; i.res = dst; i.a = src; emit(i);
    }
    std::string binary(const std::string& op, const std::string& a, const std::string& b) {
        TacInstr i; i.kind = TacKind::Binary; i.op = op; i.a = a; i.b = b; i.res = newTemp(); emit(i);
        return i.res;
    }
    void binaryInto(const std::string& dst, const std::string& op, const std::string& a, const std::string& b) {
        TacInstr i; i.kind = TacKind::Binary; i.op = op; i.a = a; i.b = b; i.res = dst; emit(i);
    }

    std::string varName(const Expr& e) const { return prog.varNames.at(e.slot); }

    static std::string doubleText(double d) {
        std::ostringstream o;
        o << d;
        std::string s = o.str();
        if (s.find_first_of(".en") == std::string::npos) s += ".0";
        return s;
    }

    // Generates code for an expression and returns the operand holding its value.
    std::string gen(const Expr& e) {
        switch (e.kind) {
            case ExprKind::IntLit:
            case ExprKind::CharLit: return std::to_string(e.ival);
            case ExprKind::FloatLit: return doubleText(e.dval);
            case ExprKind::Var: return varName(e);

            case ExprKind::Unary: {
                std::string a = gen(*e.args[0]);
                if (e.opc == Op::Plus) return a;
                TacInstr i; i.kind = TacKind::Unary; i.op = e.op; i.a = a; i.res = newTemp(); emit(i);
                return i.res;
            }

            case ExprKind::Binary: {
                std::string a = gen(*e.args[0]);
                std::string b = gen(*e.args[1]);
                return binary(e.op, a, b);
            }

            case ExprKind::Logical: {
                std::string t = newTemp();
                std::string lShort = newLabel(), lEnd = newLabel();
                bool isAnd = (e.opc == Op::LogAnd);
                std::string a = gen(*e.args[0]);
                branch(!isAnd, a, lShort);          // && : stop when false,  || : stop when true
                std::string b = gen(*e.args[1]);
                branch(!isAnd, b, lShort);
                assign(t, isAnd ? "1" : "0");
                jump(lEnd);
                label(lShort);
                assign(t, isAnd ? "0" : "1");
                label(lEnd);
                return t;
            }

            case ExprKind::Assign: {
                std::string dst = varName(*e.args[0]);
                std::string v = gen(*e.args[1]);
                if (e.opc == Op::None) assign(dst, v);
                else {
                    std::string opText = e.op.substr(0, e.op.size() - 1);
                    std::string t = binary(opText, dst, v);
                    assign(dst, t);
                }
                return dst;
            }

            case ExprKind::IncDec: {
                std::string v = varName(*e.args[0]);
                std::string opText = e.opc == Op::Add ? "+" : "-";
                if (e.prefix) {
                    binaryInto(v, opText, v, "1");
                    return v;
                }
                std::string old = newTemp();
                assign(old, v);
                binaryInto(v, opText, v, "1");
                return old;
            }

            case ExprKind::Ternary: {
                std::string t = newTemp();
                std::string lElse = newLabel(), lEnd = newLabel();
                std::string c = gen(*e.args[0]);
                branch(false, c, lElse);
                assign(t, gen(*e.args[1]));
                jump(lEnd);
                label(lElse);
                assign(t, gen(*e.args[2]));
                label(lEnd);
                return t;
            }

            case ExprKind::Cast: {
                std::string a = gen(*e.args[0]);
                TacInstr i; i.kind = TacKind::Cast; i.op = typeName(e.type); i.a = a; i.res = newTemp(); emit(i);
                return i.res;
            }

            case ExprKind::Comma:
                gen(*e.args[0]);
                return gen(*e.args[1]);
        }
        return "?";
    }

    void genStmt(const Stmt& s) {
        switch (s.kind) {
            case StmtKind::Empty: break;

            case StmtKind::Decl:
                for (auto& d : s.decls) {
                    std::string name = prog.varNames.at(d.slot);
                    if (d.init) assign(name, gen(*d.init));
                    else assign(name, "0");
                }
                break;

            case StmtKind::ExprStmt: gen(*s.expr); break;

            case StmtKind::Print: {
                TacInstr i; i.kind = TacKind::Print; i.a = gen(*s.args[0]); emit(i);
                break;
            }
            case StmtKind::Printf: {
                TacInstr i; i.kind = TacKind::Printf; i.a = s.text;
                for (auto& a : s.args) i.args.push_back(gen(*a));
                emit(i);
                break;
            }
            case StmtKind::Scanf: {
                TacInstr i; i.kind = TacKind::Scanf; i.a = s.text;
                for (auto& a : s.args) i.args.push_back(varName(*a));
                emit(i);
                break;
            }

            case StmtKind::Block:
                for (auto& c : s.stmts) genStmt(*c);
                break;

            case StmtKind::If: {
                std::string lEnd = newLabel();
                std::string c = gen(*s.cond);
                if (s.elseS) {
                    std::string lElse = newLabel();
                    branch(false, c, lElse);
                    genStmt(*s.thenS);
                    jump(lEnd);
                    label(lElse);
                    genStmt(*s.elseS);
                } else {
                    branch(false, c, lEnd);
                    genStmt(*s.thenS);
                }
                label(lEnd);
                break;
            }

            case StmtKind::While: {
                std::string lStart = newLabel(), lEnd = newLabel();
                label(lStart);
                std::string c = gen(*s.cond);
                branch(false, c, lEnd);
                breakStack.push_back(lEnd);
                continueStack.push_back(lStart);
                genStmt(*s.thenS);
                continueStack.pop_back();
                breakStack.pop_back();
                jump(lStart);
                label(lEnd);
                break;
            }

            case StmtKind::DoWhile: {
                std::string lStart = newLabel(), lCond = newLabel(), lEnd = newLabel();
                label(lStart);
                breakStack.push_back(lEnd);
                continueStack.push_back(lCond);
                genStmt(*s.thenS);
                continueStack.pop_back();
                breakStack.pop_back();
                label(lCond);
                std::string c = gen(*s.cond);
                branch(true, c, lStart);
                label(lEnd);
                break;
            }

            case StmtKind::For: {
                std::string lCond = newLabel(), lStep = newLabel(), lEnd = newLabel();
                if (s.init) genStmt(*s.init);
                label(lCond);
                if (s.cond) {
                    std::string c = gen(*s.cond);
                    branch(false, c, lEnd);
                }
                breakStack.push_back(lEnd);
                continueStack.push_back(lStep);
                genStmt(*s.thenS);
                continueStack.pop_back();
                breakStack.pop_back();
                label(lStep);
                if (s.step) gen(*s.step);
                jump(lCond);
                label(lEnd);
                break;
            }

            case StmtKind::Switch: {
                std::string v = gen(*s.cond);
                std::string lEnd = newLabel();
                std::vector<std::string> caseLabels;
                std::string lDefault = lEnd;
                for (auto& c : s.cases) {
                    caseLabels.push_back(newLabel());
                    if (c.isDefault) lDefault = caseLabels.back();
                }
                for (size_t i = 0; i < s.cases.size(); ++i) {
                    if (s.cases[i].isDefault) continue;
                    std::string t = binary("==", v, std::to_string(s.cases[i].value));
                    branch(true, t, caseLabels[i]);
                }
                jump(lDefault);
                breakStack.push_back(lEnd);
                for (size_t i = 0; i < s.cases.size(); ++i) {
                    label(caseLabels[i]);
                    for (auto& st : s.cases[i].body) genStmt(*st);
                }
                breakStack.pop_back();
                label(lEnd);
                break;
            }

            case StmtKind::Break: jump(breakStack.back()); break;
            case StmtKind::Continue: jump(continueStack.back()); break;

            case StmtKind::Return: {
                TacInstr i; i.kind = TacKind::Return;
                if (s.expr) i.a = gen(*s.expr);
                emit(i);
                break;
            }
        }
    }

public:
    explicit TacGen(const Program& p) : prog(p) {}
    std::vector<TacInstr> run() {
        genStmt(*prog.body);
        return out;
    }
};

}  // namespace

std::vector<TacInstr> generateTAC(const Program& program) { return TacGen(program).run(); }

std::string formatTAC(const TacInstr& i) {
    switch (i.kind) {
        case TacKind::Assign:  return "    " + i.res + " = " + i.a;
        case TacKind::Binary:  return "    " + i.res + " = " + i.a + " " + i.op + " " + i.b;
        case TacKind::Unary:   return "    " + i.res + " = " + i.op + i.a;
        case TacKind::Cast:    return "    " + i.res + " = (" + i.op + ") " + i.a;
        case TacKind::Label:   return i.res + ":";
        case TacKind::Goto:    return "    goto " + i.res;
        case TacKind::IfFalse: return "    ifFalse " + i.a + " goto " + i.res;
        case TacKind::IfTrue:  return "    if " + i.a + " goto " + i.res;
        case TacKind::Print:   return "    print " + i.a;
        case TacKind::Printf: {
            std::string s = "    printf \"" + escapeText(i.a) + "\"";
            for (auto& a : i.args) s += ", " + a;
            return s;
        }
        case TacKind::Scanf: {
            std::string s = "    scanf \"" + escapeText(i.a) + "\"";
            for (auto& a : i.args) s += ", &" + a;
            return s;
        }
        case TacKind::Return:  return i.a.empty() ? "    return" : "    return " + i.a;
    }
    return "";
}
