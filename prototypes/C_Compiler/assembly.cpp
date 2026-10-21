#include "assembly.h"
#include <cctype>
#include <set>
#include <sstream>

namespace {

bool isImmediate(const std::string& s) {
    return !s.empty() && (std::isdigit((unsigned char)s[0]) || s[0] == '-' || s[0] == '.');
}
std::string mem(const std::string& s) { return isImmediate(s) ? s : "[" + s + "]"; }

bool isTemp(const std::string& s) {
    if (s.size() < 2 || s[0] != 't') return false;
    for (size_t i = 1; i < s.size(); ++i)
        if (!std::isdigit((unsigned char)s[i])) return false;
    return true;
}

// "text\n" -> "text", 10, 0   (NASM data string)
std::string asmString(const std::string& s) {
    std::vector<std::string> parts;
    std::string cur;
    for (char c : s) {
        if (c >= 32 && c < 127 && c != '"') cur += c;
        else {
            if (!cur.empty()) { parts.push_back("\"" + cur + "\""); cur.clear(); }
            parts.push_back(std::to_string((int)(unsigned char)c));
        }
    }
    if (!cur.empty()) parts.push_back("\"" + cur + "\"");
    parts.push_back("0");
    std::string r;
    for (size_t i = 0; i < parts.size(); ++i) r += (i ? ", " : "") + parts[i];
    return r;
}

const char* dataDirective(Type t) {
    switch (t) {
        case Type::Char: return "DB";
        case Type::Long:
        case Type::Double: return "DQ";
        default: return "DD";
    }
}

}  // namespace

std::string generateAssembly(const std::vector<TacInstr>& tac, const Program& program) {
    std::ostringstream o;

    // ---- collect temporaries and format strings
    std::set<std::string> temps;
    std::vector<std::string> strings;
    for (auto& i : tac) {
        if (isTemp(i.res)) temps.insert(i.res);
        if (i.kind == TacKind::Printf || i.kind == TacKind::Scanf) strings.push_back(i.a);
    }

    o << "; Lexify pseudo assembly (x86-like, 32-bit integer view - meant for reading, not for assembling)\n";
    o << "section .data\n";
    o << "    fmt_int: DB \"%d\", 10, 0\n";
    for (size_t k = 0; k < strings.size(); ++k) o << "    str" << k << ": DB " << asmString(strings[k]) << "\n";
    for (size_t k = 0; k < program.varNames.size(); ++k)
        o << "    " << program.varNames[k] << ": " << dataDirective(program.varTypes[k]) << " 0\n";
    for (auto& t : temps) o << "    " << t << ": DD 0\n";

    o << "\nsection .text\n_main:\n";

    size_t strIndex = 0;
    auto ins = [&](const std::string& s) { o << "    " << s << "\n"; };

    for (auto& i : tac) {
        o << "    ; " << (i.kind == TacKind::Label ? i.res + ":" : formatTAC(i).substr(4)) << "\n";
        switch (i.kind) {
            case TacKind::Assign:
                ins("MOV EAX, " + mem(i.a));
                ins("MOV [" + i.res + "], EAX");
                break;

            case TacKind::Binary: {
                ins("MOV EAX, " + mem(i.a));
                const std::string& op = i.op;
                if (op == "+") ins("ADD EAX, " + mem(i.b));
                else if (op == "-") ins("SUB EAX, " + mem(i.b));
                else if (op == "*") ins("IMUL EAX, " + mem(i.b));
                else if (op == "/" || op == "%") {
                    ins("MOV EBX, " + mem(i.b));
                    ins("CDQ");
                    ins("IDIV EBX");
                    if (op == "%") ins("MOV EAX, EDX");
                } else if (op == "&") ins("AND EAX, " + mem(i.b));
                else if (op == "|") ins("OR EAX, " + mem(i.b));
                else if (op == "^") ins("XOR EAX, " + mem(i.b));
                else if (op == "<<" || op == ">>") {
                    ins("MOV ECX, " + mem(i.b));
                    ins(std::string(op == "<<" ? "SHL" : "SAR") + " EAX, CL");
                } else {
                    const char* set = op == "==" ? "SETE" : op == "!=" ? "SETNE" : op == "<" ? "SETL"
                                    : op == "<=" ? "SETLE" : op == ">" ? "SETG" : "SETGE";
                    ins("CMP EAX, " + mem(i.b));
                    ins(std::string(set) + " AL");
                    ins("MOVZX EAX, AL");
                }
                ins("MOV [" + i.res + "], EAX");
                break;
            }

            case TacKind::Unary:
                ins("MOV EAX, " + mem(i.a));
                if (i.op == "-") ins("NEG EAX");
                else if (i.op == "~") ins("NOT EAX");
                else {  // '!'
                    ins("CMP EAX, 0");
                    ins("SETE AL");
                    ins("MOVZX EAX, AL");
                }
                ins("MOV [" + i.res + "], EAX");
                break;

            case TacKind::Cast:
                ins("MOV EAX, " + mem(i.a) + "        ; convert to " + i.op);
                ins("MOV [" + i.res + "], EAX");
                break;

            case TacKind::Label: o << i.res << ":\n"; break;
            case TacKind::Goto: ins("JMP " + i.res); break;

            case TacKind::IfFalse:
            case TacKind::IfTrue:
                ins("MOV EAX, " + mem(i.a));
                ins("CMP EAX, 0");
                ins(std::string(i.kind == TacKind::IfFalse ? "JE " : "JNE ") + i.res);
                break;

            case TacKind::Print:
                ins("PUSH " + mem(i.a));
                ins("PUSH fmt_int");
                ins("CALL printf");
                ins("ADD ESP, 8");
                break;

            case TacKind::Printf: {
                for (size_t k = i.args.size(); k-- > 0;) ins("PUSH " + mem(i.args[k]));
                ins("PUSH str" + std::to_string(strIndex++));
                ins("CALL printf");
                ins("ADD ESP, " + std::to_string(4 * (i.args.size() + 1)));
                break;
            }

            case TacKind::Scanf: {
                for (size_t k = i.args.size(); k-- > 0;) ins("PUSH OFFSET " + i.args[k]);
                ins("PUSH str" + std::to_string(strIndex++));
                ins("CALL scanf");
                ins("ADD ESP, " + std::to_string(4 * (i.args.size() + 1)));
                break;
            }

            case TacKind::Return:
                if (!i.a.empty()) ins("MOV EAX, " + mem(i.a));
                ins("JMP _exit");
                break;
        }
    }
    o << "_exit:\n    RET\n";
    return o.str();
}
