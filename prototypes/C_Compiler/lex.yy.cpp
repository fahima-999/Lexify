// lex.yy.cpp - hand written lexer (the flex file lexer.l is only a reference).
#include "lexer.h"
#include <cctype>
#include <climits>
#include <map>
#include <set>

namespace {

using MacroTable = std::map<std::string, std::vector<Token>>;

const std::set<std::string> kKeywords = {
    "int", "char", "float", "double", "long", "void",
    "if", "else", "while", "do", "for", "switch", "case", "default",
    "break", "continue", "return",
    "print", "printf", "scanf"};

// s[i] is the character right after a backslash. Returns the decoded character.
int readEscape(const std::string& s, size_t& i, int line) {
    if (i >= s.size()) lexError(line, "unfinished escape sequence");
    char c = s[i++];
    switch (c) {
        case 'n': return '\n';
        case 't': return '\t';
        case 'r': return '\r';
        case '0': return 0;
        case 'a': return '\a';
        case 'b': return '\b';
        case 'f': return '\f';
        case 'v': return '\v';
        case '\\': return '\\';
        case '\'': return '\'';
        case '"': return '"';
    }
    lexError(line, std::string("unknown escape sequence '\\") + c + "'");
}

bool isIdStart(char c) { return std::isalpha((unsigned char)c) || c == '_'; }
bool isIdChar(char c) { return std::isalnum((unsigned char)c) || c == '_'; }

void lexInto(const std::string& s, int line, std::vector<Token>& out, MacroTable& macros) {
    static const char* ops3[] = {"<<=", ">>="};
    static const char* ops2[] = {"++", "--", "+=", "-=", "*=", "/=", "%=", "&=", "|=", "^=",
                                 "<<", ">>", "<=", ">=", "==", "!=", "&&", "||"};
    const std::string singles = "+-*/%=<>!~&|^?:;,(){}";

    size_t i = 0;
    while (i < s.size()) {
        char ch = s[i];

        // ---- whitespace
        if (ch == '\n') { ++line; ++i; continue; }
        if (std::isspace((unsigned char)ch)) { ++i; continue; }

        // ---- comments
        if (ch == '/' && i + 1 < s.size() && s[i + 1] == '/') {
            while (i < s.size() && s[i] != '\n') ++i;
            continue;
        }
        if (ch == '/' && i + 1 < s.size() && s[i + 1] == '*') {
            int startLine = line;
            i += 2;
            while (i + 1 < s.size() && !(s[i] == '*' && s[i + 1] == '/')) {
                if (s[i] == '\n') ++line;
                ++i;
            }
            if (i + 1 >= s.size()) lexError(startLine, "unterminated /* comment");
            i += 2;
            continue;
        }

        // ---- preprocessor lines
        if (ch == '#') {
            size_t e = s.find('\n', i);
            if (e == std::string::npos) e = s.size();
            std::string text = s.substr(i + 1, e - i - 1);
            size_t p = 0;
            while (p < text.size() && std::isspace((unsigned char)text[p])) ++p;
            size_t q = p;
            while (q < text.size() && isIdChar(text[q])) ++q;
            std::string word = text.substr(p, q - p);
            if (word == "include" || word == "pragma") {
                // ignored: printf/scanf are built in
            } else if (word == "define") {
                while (q < text.size() && std::isspace((unsigned char)text[q])) ++q;
                size_t n = q;
                while (n < text.size() && isIdChar(text[n])) ++n;
                std::string name = text.substr(q, n - q);
                if (name.empty() || !isIdStart(name[0])) lexError(line, "#define needs a macro name");
                if (kKeywords.count(name)) lexError(line, "cannot #define the keyword '" + name + "'");
                if (n < text.size() && text[n] == '(') lexError(line, "function-like macros are not supported");
                std::vector<Token> body;
                lexInto(text.substr(n), line, body, macros);
                macros[name] = body;
            } else {
                lexError(line, "unsupported preprocessor directive '#" + word + "'");
            }
            i = e;
            continue;
        }

        // ---- identifiers, keywords, macros
        if (isIdStart(ch)) {
            size_t j = i++;
            while (i < s.size() && isIdChar(s[i])) ++i;
            std::string word = s.substr(j, i - j);
            if (kKeywords.count(word)) {
                out.push_back({word, word, line});
            } else if (macros.count(word)) {
                for (Token t : macros[word]) {
                    t.line = line;
                    out.push_back(t);
                }
            } else {
                out.push_back({"ID", word, line});
            }
            continue;
        }

        // ---- numbers
        if (std::isdigit((unsigned char)ch) || (ch == '.' && i + 1 < s.size() && std::isdigit((unsigned char)s[i + 1]))) {
            size_t j = i;
            if (ch == '0' && j + 1 < s.size() && (s[j + 1] == 'x' || s[j + 1] == 'X')) {
                j += 2;
                size_t st = j;
                while (j < s.size() && std::isxdigit((unsigned char)s[j])) ++j;
                if (j == st) lexError(line, "invalid hexadecimal literal");
                unsigned long long v = 0;
                try { v = std::stoull(s.substr(st, j - st), nullptr, 16); }
                catch (...) { lexError(line, "integer literal too large"); }
                if (v > (unsigned long long)LLONG_MAX) lexError(line, "integer literal too large");
                bool isLong = false;
                while (j < s.size() && (s[j] == 'l' || s[j] == 'L' || s[j] == 'u' || s[j] == 'U')) {
                    if (s[j] == 'l' || s[j] == 'L') isLong = true;
                    ++j;
                }
                out.push_back({isLong ? "LONG_LIT" : "INT_LIT", std::to_string(v), line});
            } else {
                while (j < s.size() && std::isdigit((unsigned char)s[j])) ++j;
                bool isFloat = false;
                if (j < s.size() && s[j] == '.') {
                    isFloat = true;
                    ++j;
                    while (j < s.size() && std::isdigit((unsigned char)s[j])) ++j;
                }
                if (j < s.size() && (s[j] == 'e' || s[j] == 'E')) {
                    size_t k = j + 1;
                    if (k < s.size() && (s[k] == '+' || s[k] == '-')) ++k;
                    if (k < s.size() && std::isdigit((unsigned char)s[k])) {
                        isFloat = true;
                        j = k;
                        while (j < s.size() && std::isdigit((unsigned char)s[j])) ++j;
                    }
                }
                std::string body = s.substr(i, j - i);
                if (isFloat) {
                    std::string kind = "FLOAT_LIT";
                    if (j < s.size() && (s[j] == 'f' || s[j] == 'F')) { kind = "FLOATF_LIT"; ++j; }
                    else if (j < s.size() && (s[j] == 'l' || s[j] == 'L')) ++j;
                    out.push_back({kind, body, line});
                } else {
                    bool isLong = false;
                    while (j < s.size() && (s[j] == 'l' || s[j] == 'L' || s[j] == 'u' || s[j] == 'U')) {
                        if (s[j] == 'l' || s[j] == 'L') isLong = true;
                        ++j;
                    }
                    int base = 10;
                    if (body.size() > 1 && body[0] == '0') {
                        base = 8;
                        for (char d : body)
                            if (d > '7') lexError(line, "invalid digit '" + std::string(1, d) + "' in octal literal");
                    }
                    unsigned long long v = 0;
                    try { v = std::stoull(body, nullptr, base); }
                    catch (...) { lexError(line, "integer literal too large"); }
                    if (v > (unsigned long long)LLONG_MAX) lexError(line, "integer literal too large");
                    out.push_back({isLong ? "LONG_LIT" : "INT_LIT", std::to_string(v), line});
                }
            }
            if (j < s.size() && isIdChar(s[j])) lexError(line, "invalid suffix on numeric literal");
            i = j;
            continue;
        }

        // ---- character literal
        if (ch == '\'') {
            ++i;
            if (i >= s.size() || s[i] == '\n' || s[i] == '\'') lexError(line, "invalid character literal");
            int v;
            if (s[i] == '\\') { ++i; v = readEscape(s, i, line); }
            else v = (signed char)s[i++];
            if (i >= s.size() || s[i] != '\'') lexError(line, "character literal must contain exactly one character");
            ++i;
            out.push_back({"CHAR_LIT", std::to_string(v), line});
            continue;
        }

        // ---- string literal (only meaningful inside printf / scanf)
        if (ch == '"') {
            ++i;
            std::string v;
            while (true) {
                if (i >= s.size() || s[i] == '\n') lexError(line, "unterminated string literal");
                if (s[i] == '"') { ++i; break; }
                if (s[i] == '\\') { ++i; v += (char)readEscape(s, i, line); }
                else v += s[i++];
            }
            out.push_back({"STRING", v, line});
            continue;
        }

        // ---- operators and punctuation (longest match first)
        bool done = false;
        if (i + 3 <= s.size()) {
            for (auto op : ops3)
                if (s.compare(i, 3, op) == 0) {
                    out.push_back({op, op, line});
                    i += 3;
                    done = true;
                    break;
                }
        }
        if (done) continue;
        if (i + 2 <= s.size()) {
            for (auto op : ops2)
                if (s.compare(i, 2, op) == 0) {
                    out.push_back({op, op, line});
                    i += 2;
                    done = true;
                    break;
                }
        }
        if (done) continue;
        if (singles.find(ch) != std::string::npos) {
            out.push_back({std::string(1, ch), std::string(1, ch), line});
            ++i;
            continue;
        }

        if (ch == '[' || ch == ']') lexError(line, "arrays are not supported by this compiler");
        lexError(line, std::string("unexpected character '") + ch + "'");
    }
}

}  // namespace

std::vector<Token> tokenize(const std::string& source) {
    std::vector<Token> out;
    MacroTable macros;
    lexInto(source, 1, out, macros);
    int lastLine = out.empty() ? 1 : out.back().line;
    // line of EOF = number of lines in the file
    int lines = 1;
    for (char c : source)
        if (c == '\n') ++lines;
    out.push_back({"EOF", "", lines > lastLine ? lines : lastLine});
    return out;
}
