#pragma once
#include "expr.h"
#include <string>
#include <vector>

namespace ppl {

class Parser {
    std::string s;
    std::size_t pos;
public:
    explicit Parser(const std::string& str);
    Expr parse_expr();
    std::vector<Expr> parse_all();
private:
    void skip();
    Expr parse_list();
    Expr parse_list_bracket();
    Expr parse_string();
    Expr parse_atom();
};

std::vector<Expr> parse(const std::string& s);

} // namespace ppl