#include "parser.h"
#include <cctype>

namespace ppl {

Parser::Parser(const std::string& str) : s(str), pos(0) {}

void Parser::skip() {
    while (pos < s.size() && std::isspace(static_cast<unsigned char>(s[pos]))) ++pos;
}

Expr Parser::parse_expr() {
    skip();
    if (s[pos] == '(') return parse_list();
    if (s[pos] == '[') return parse_list_bracket();
    if (s[pos] == '"') return parse_string();
    return parse_atom();
}

Expr Parser::parse_list() {
    ++pos; skip();
    std::vector<Expr> items;
    while (pos < s.size() && s[pos] != ')') {
        items.push_back(parse_expr());
        skip();
    }
    ++pos;
    return Expr(std::move(items));
}

Expr Parser::parse_list_bracket() {
    ++pos; skip();
    std::vector<Expr> items;
    while (pos < s.size() && s[pos] != ']') {
        items.push_back(parse_expr());
        skip();
    }
    ++pos;
    return Expr(std::move(items));
}

Expr Parser::parse_string() {
    ++pos;
    std::string r;
    while (pos < s.size() && s[pos] != '"') r += s[pos++];
    ++pos;
    return Expr(Symbol(r));
}

Expr Parser::parse_atom() {
    skip();
    std::string r;
    bool is_num = true, has_dot = false;
    if (s[pos] == '-' || s[pos] == '+') r += s[pos++];
    while (pos < s.size() && !std::isspace(static_cast<unsigned char>(s[pos]))
           && s[pos] != '(' && s[pos] != ')' && s[pos] != '[' && s[pos] != ']') {
        if (!std::isdigit(static_cast<unsigned char>(s[pos])) && s[pos] != '.') is_num = false;
        if (s[pos] == '.') { if (has_dot) is_num = false; has_dot = true; }
        r += s[pos++];
    }
    if (r == "-" || r == "+") is_num = false;
    if (is_num && !r.empty()) return Expr(std::stod(r));
    return Expr(Symbol(r));
}

std::vector<Expr> Parser::parse_all() {
    std::vector<Expr> res;
    skip();
    while (pos < s.size()) {
        res.push_back(parse_expr());
        skip();
    }
    return res;
}

std::vector<Expr> parse(const std::string& s) {
    return Parser(s).parse_all();
}

} // namespace ppl