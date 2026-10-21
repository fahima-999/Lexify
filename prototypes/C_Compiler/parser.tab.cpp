// parser.tab.cpp - hand written recursive descent parser (parser.y is only a reference grammar).
//
// program     -> { top_item } EOF
// top_item    -> main_function | statement
// main_function -> ("int"|"void") "main" "(" ["void"] ")" block
#include "parser.tab.h"
#include <algorithm>
#include <climits>
#include <set>

namespace {

class Parser {
    const std::vector<Token>& ts;
    size_t pos = 0;

    // ---------------------------------------------------------- token helpers
    const Token& peek(size_t k = 0) const { return ts[std::min(pos + k, ts.size() - 1)]; }
    const Token& prev() const { return ts[pos ? pos - 1 : 0]; }
    bool check(const char* k) const { return peek().kind == k; }
    Token advance() {
        Token t = peek();
        if (pos < ts.size() - 1) ++pos;
        return t;
    }
    bool match(const char* k) {
        if (check(k)) { advance(); return true; }
        return false;
    }
    static std::string describe(const Token& t) {
        if (t.kind == "EOF") return "end of file";
        if (t.kind == "STRING") return "a string literal";
        return "'" + t.text + "'";
    }
    Token expect(const char* k) {
        if (!check(k)) {
            // a missing ';' is best reported on the line where the statement ended
            int ln = (std::string(k) == ";") ? prev().line : peek().line;
            syntaxError(ln, std::string("expected '") + k + "' but found " + describe(peek()));
        }
        return advance();
    }
    static bool isTypeStart(const std::string& k) {
        return k == "int" || k == "char" || k == "float" || k == "double" || k == "long" || k == "void";
    }

    // ------------------------------------------------------------------ types
    Type parseType() {
        Token t = advance();
        if (t.kind == "int") return Type::Int;
        if (t.kind == "char") return Type::Char;
        if (t.kind == "float") return Type::Float;
        if (t.kind == "double") return Type::Double;
        if (t.kind == "void") return Type::Void;
        // long, long long, long int, long long int
        while (check("long") || check("int")) advance();
        return Type::Long;
    }

    // ----------------------------------------------------------- expressions
    ExprPtr parseExpression() {  // comma expression
        auto e = parseAssignment();
        while (check(",")) {
            int ln = advance().line;
            auto c = makeExpr(ExprKind::Comma, ln);
            c->op = ",";
            c->args = {e, parseAssignment()};
            e = c;
        }
        return e;
    }

    static bool isAssignOp(const std::string& k) {
        static const std::set<std::string> ops = {"=", "+=", "-=", "*=", "/=", "%=", "&=", "|=", "^=", "<<=", ">>="};
        return ops.count(k) > 0;
    }

    ExprPtr parseAssignment() {
        auto lhs = parseTernary();
        if (isAssignOp(peek().kind)) {
            Token op = advance();
            if (lhs->kind != ExprKind::Var) syntaxError(op.line, "the left side of '" + op.text + "' must be a variable");
            auto rhs = parseAssignment();  // right associative
            auto a = makeExpr(ExprKind::Assign, op.line);
            a->op = op.text;
            a->opc = (op.text == "=") ? Op::None : binaryOpFromText(op.text.substr(0, op.text.size() - 1));
            a->args = {lhs, rhs};
            return a;
        }
        return lhs;
    }

    ExprPtr parseTernary() {
        auto c = parseBinary(1);
        if (check("?")) {
            int ln = advance().line;
            auto a = parseExpression();
            expect(":");
            auto b = parseTernary();
            auto t = makeExpr(ExprKind::Ternary, ln);
            t->op = "?:";
            t->args = {c, a, b};
            return t;
        }
        return c;
    }

    static int precedence(const std::string& k) {
        if (k == "||") return 1;
        if (k == "&&") return 2;
        if (k == "|") return 3;
        if (k == "^") return 4;
        if (k == "&") return 5;
        if (k == "==" || k == "!=") return 6;
        if (k == "<" || k == "<=" || k == ">" || k == ">=") return 7;
        if (k == "<<" || k == ">>") return 8;
        if (k == "+" || k == "-") return 9;
        if (k == "*" || k == "/" || k == "%") return 10;
        return 0;
    }

    ExprPtr parseBinary(int minPrec) {
        auto lhs = parseUnary();
        while (true) {
            int p = precedence(peek().kind);
            if (p == 0 || p < minPrec) break;
            Token op = advance();
            auto rhs = parseBinary(p + 1);  // left associative
            bool logical = (op.kind == "&&" || op.kind == "||");
            auto e = makeExpr(logical ? ExprKind::Logical : ExprKind::Binary, op.line);
            e->op = op.text;
            e->opc = binaryOpFromText(op.text);
            e->args = {lhs, rhs};
            lhs = e;
        }
        return lhs;
    }

    ExprPtr parseUnary() {
        const Token& t = peek();
        if (t.kind == "+" || t.kind == "-" || t.kind == "!" || t.kind == "~") {
            Token op = advance();
            auto e = makeExpr(ExprKind::Unary, op.line);
            e->op = op.text;
            e->opc = op.text == "+" ? Op::Plus : op.text == "-" ? Op::Neg : op.text == "!" ? Op::Not : Op::BitNot;
            e->args = {parseUnary()};
            return e;
        }
        if (t.kind == "++" || t.kind == "--") {
            Token op = advance();
            auto operand = parseUnary();
            if (operand->kind != ExprKind::Var) syntaxError(op.line, "'" + op.text + "' needs a variable");
            auto e = makeExpr(ExprKind::IncDec, op.line);
            e->op = op.text;
            e->opc = op.text == "++" ? Op::Add : Op::Sub;
            e->prefix = true;
            e->args = {operand};
            return e;
        }
        if (t.kind == "*" ) syntaxError(t.line, "pointers are not supported by this compiler");
        if (t.kind == "(" && isTypeStart(peek(1).kind)) {  // cast
            int ln = advance().line;
            Type ty = parseType();
            if (ty == Type::Void) syntaxError(ln, "cannot cast to void");
            if (check("*")) syntaxError(peek().line, "pointers are not supported by this compiler");
            expect(")");
            auto e = makeExpr(ExprKind::Cast, ln);
            e->op = "cast";
            e->type = ty;
            e->args = {parseUnary()};
            return e;
        }
        return parsePostfix();
    }

    ExprPtr parsePostfix() {
        auto e = parsePrimary();
        while (check("++") || check("--")) {
            Token op = advance();
            if (e->kind != ExprKind::Var) syntaxError(op.line, "'" + op.text + "' needs a variable");
            auto p = makeExpr(ExprKind::IncDec, op.line);
            p->op = op.text;
            p->opc = op.text == "++" ? Op::Add : Op::Sub;
            p->prefix = false;
            p->args = {e};
            e = p;
        }
        return e;
    }

    ExprPtr parsePrimary() {
        Token t = peek();
        if (t.kind == "INT_LIT" || t.kind == "LONG_LIT") {
            advance();
            auto e = makeExpr(ExprKind::IntLit, t.line);
            e->ival = std::stoll(t.text);
            e->type = (t.kind == "LONG_LIT" || e->ival > INT_MAX) ? Type::Long : Type::Int;
            return e;
        }
        if (t.kind == "CHAR_LIT") {
            advance();
            auto e = makeExpr(ExprKind::CharLit, t.line);
            e->ival = std::stoll(t.text);
            e->type = Type::Int;  // like C: 'a' has type int
            return e;
        }
        if (t.kind == "FLOAT_LIT" || t.kind == "FLOATF_LIT") {
            advance();
            auto e = makeExpr(ExprKind::FloatLit, t.line);
            e->dval = std::stod(t.text);
            e->type = t.kind == "FLOATF_LIT" ? Type::Float : Type::Double;
            return e;
        }
        if (t.kind == "ID") {
            advance();
            if (check("(")) syntaxError(t.line, "function calls are not supported by this compiler ('" + t.text + "')");
            auto e = makeExpr(ExprKind::Var, t.line);
            e->name = t.text;
            return e;
        }
        if (t.kind == "(") {
            advance();
            auto e = parseExpression();
            expect(")");
            return e;
        }
        if (t.kind == "STRING") syntaxError(t.line, "string literals are only allowed as the format of printf/scanf");
        syntaxError(t.line, "expected an expression but found " + describe(t));
    }

    // ------------------------------------------------------------- statements
    StmtPtr parseDeclaration() {  // does NOT consume the ';'
        int ln = peek().line;
        Type ty = parseType();
        if (ty == Type::Void) syntaxError(ln, "a variable cannot have type void");
        auto s = makeStmt(StmtKind::Decl, ln);
        s->declType = ty;
        do {
            if (check("*")) syntaxError(peek().line, "pointers are not supported by this compiler");
            Token name = expect("ID");
            if (check("(")) syntaxError(name.line, "functions are not supported (only main)");
            if (check("[")) syntaxError(name.line, "arrays are not supported by this compiler");
            Declarator d;
            d.name = name.text;
            d.line = name.line;
            if (match("=")) d.init = parseAssignment();
            s->decls.push_back(d);
        } while (match(","));
        return s;
    }

    StmtPtr parseBlock() {
        auto b = makeStmt(StmtKind::Block, expect("{").line);
        while (!check("}")) {
            if (check("EOF")) syntaxError(peek().line, "missing '}' - the block was never closed");
            b->stmts.push_back(parseStatement());
        }
        expect("}");
        return b;
    }

    StmtPtr parseBody() {  // body of if / else / loops
        if (isTypeStart(peek().kind))
            syntaxError(peek().line, "a declaration cannot be the body of if/else/loop - put it inside { }");
        return parseStatement();
    }

    StmtPtr parseIf() {
        auto s = makeStmt(StmtKind::If, advance().line);
        expect("(");
        s->cond = parseExpression();
        expect(")");
        s->thenS = parseBody();
        if (match("else")) s->elseS = parseBody();  // else binds to the nearest if
        return s;
    }

    StmtPtr parseWhile() {
        auto s = makeStmt(StmtKind::While, advance().line);
        expect("(");
        s->cond = parseExpression();
        expect(")");
        s->thenS = parseBody();
        return s;
    }

    StmtPtr parseDoWhile() {
        auto s = makeStmt(StmtKind::DoWhile, advance().line);
        s->thenS = parseBody();
        expect("while");
        expect("(");
        s->cond = parseExpression();
        expect(")");
        expect(";");
        return s;
    }

    StmtPtr parseFor() {
        auto s = makeStmt(StmtKind::For, advance().line);
        expect("(");
        if (!check(";")) {
            if (isTypeStart(peek().kind)) {
                s->init = parseDeclaration();
            } else {
                auto e = makeStmt(StmtKind::ExprStmt, peek().line);
                e->expr = parseExpression();
                s->init = e;
            }
        }
        expect(";");
        if (!check(";")) s->cond = parseExpression();
        expect(";");
        if (!check(")")) s->step = parseExpression();
        expect(")");
        s->thenS = parseBody();
        return s;
    }

    StmtPtr parseSwitch() {
        auto s = makeStmt(StmtKind::Switch, advance().line);
        expect("(");
        s->cond = parseExpression();
        expect(")");
        expect("{");
        while (!check("}")) {
            if (check("EOF")) syntaxError(peek().line, "missing '}' at the end of switch");
            if (check("case")) {
                SwitchCase c;
                c.line = advance().line;
                c.valueExpr = parseTernary();
                expect(":");
                s->cases.push_back(c);
            } else if (check("default")) {
                SwitchCase c;
                c.line = advance().line;
                c.isDefault = true;
                expect(":");
                s->cases.push_back(c);
            } else {
                if (s->cases.empty()) syntaxError(peek().line, "a statement inside switch must come after a 'case' or 'default' label");
                s->cases.back().body.push_back(parseStatement());
            }
        }
        expect("}");
        return s;
    }

    StmtPtr parsePrintCall() {  // print(expr);
        auto s = makeStmt(StmtKind::Print, advance().line);
        expect("(");
        s->args.push_back(parseAssignment());
        expect(")");
        expect(";");
        return s;
    }

    StmtPtr parsePrintf() {  // printf("fmt", a, b);
        auto s = makeStmt(StmtKind::Printf, advance().line);
        expect("(");
        s->text = expect("STRING").text;
        while (match(",")) s->args.push_back(parseAssignment());
        expect(")");
        expect(";");
        return s;
    }

    StmtPtr parseScanf() {  // scanf("%d", &x);
        auto s = makeStmt(StmtKind::Scanf, advance().line);
        expect("(");
        s->text = expect("STRING").text;
        while (match(",")) {
            if (!check("&")) syntaxError(peek().line, "scanf arguments must be written as &variable");
            advance();
            Token id = expect("ID");
            auto v = makeExpr(ExprKind::Var, id.line);
            v->name = id.text;
            s->args.push_back(v);
        }
        expect(")");
        expect(";");
        return s;
    }

    StmtPtr parseStatement() {
        const std::string& k = peek().kind;
        int ln = peek().line;
        if (k == ";") { advance(); return makeStmt(StmtKind::Empty, ln); }
        if (k == "{") return parseBlock();
        if (isTypeStart(k)) {
            auto d = parseDeclaration();
            expect(";");
            return d;
        }
        if (k == "if") return parseIf();
        if (k == "while") return parseWhile();
        if (k == "do") return parseDoWhile();
        if (k == "for") return parseFor();
        if (k == "switch") return parseSwitch();
        if (k == "break") { advance(); expect(";"); return makeStmt(StmtKind::Break, ln); }
        if (k == "continue") { advance(); expect(";"); return makeStmt(StmtKind::Continue, ln); }
        if (k == "return") {
            advance();
            auto s = makeStmt(StmtKind::Return, ln);
            if (!check(";")) s->expr = parseExpression();
            expect(";");
            return s;
        }
        if (k == "print") return parsePrintCall();
        if (k == "printf") return parsePrintf();
        if (k == "scanf") return parseScanf();
        if (k == "else") syntaxError(ln, "'else' without a matching 'if'");
        if (k == "case" || k == "default") syntaxError(ln, "'" + k + "' outside of a switch");
        if (k == "}") syntaxError(ln, "unexpected '}'");
        auto s = makeStmt(StmtKind::ExprStmt, ln);
        s->expr = parseExpression();
        expect(";");
        return s;
    }

    bool atMainHeader() const {
        return (check("int") || check("void")) && peek(1).kind == "ID" && peek(1).text == "main" && peek(2).kind == "(";
    }

public:
    explicit Parser(const std::vector<Token>& t) : ts(t) {}

    Program program() {
        Program prog;
        auto body = makeStmt(StmtKind::Block, 1);
        while (!check("EOF")) {
            if (atMainHeader()) {
                advance();  // int / void
                advance();  // main
                expect("(");
                match("void");
                expect(")");
                body->stmts.push_back(parseBlock());
                prog.hasMain = true;
                if (!check("EOF"))
                    syntaxError(peek().line, "unexpected " + describe(peek()) + " after main() - only a single main function is supported");
                break;
            }
            body->stmts.push_back(parseStatement());
        }
        prog.body = body;
        return prog;
    }
};

}  // namespace

Program parse(const std::vector<Token>& tokens) { return Parser(tokens).program(); }
