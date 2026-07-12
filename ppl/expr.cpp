#include "expr.h"

namespace ppl {

Expr::Expr(double d) : data(d) {}
Expr::Expr(const Symbol& s) : data(s) {}
Expr::Expr(Symbol&& s) : data(std::move(s)) {}
Expr::Expr(const List& l) : data(std::make_shared<List>(l)) {}
Expr::Expr(List&& l) : data(std::make_shared<List>(std::move(l))) {}
Expr::Expr(const std::vector<Expr>& items) : data(std::make_shared<List>(List{items})) {}
Expr::Expr(std::vector<Expr>&& items) : data(std::make_shared<List>(List{std::move(items)})) {}
Expr::Expr(std::initializer_list<Expr> init) : data(std::make_shared<List>(List{std::vector<Expr>(init)})) {}

bool Expr::is_number() const { return std::holds_alternative<double>(data); }
bool Expr::is_symbol() const { return std::holds_alternative<Symbol>(data); }
bool Expr::is_list() const { return std::holds_alternative<std::shared_ptr<List>>(data); }

double Expr::as_number() const { return std::get<double>(data); }
const Symbol& Expr::as_symbol() const { return std::get<Symbol>(data); }
const Expr::List& Expr::as_list() const { return *std::get<std::shared_ptr<List>>(data); }
Expr::List& Expr::as_list() { return *std::get<std::shared_ptr<List>>(data); }

size_t Expr::size() const { return is_list() ? as_list().items.size() : 0; }
const Expr& Expr::operator[](size_t i) const { return as_list().items[i]; }
Expr& Expr::operator[](size_t i) { return as_list().items[i]; }

} // namespace ppl