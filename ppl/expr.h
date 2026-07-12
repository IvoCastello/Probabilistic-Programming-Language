#pragma once
#include <cstddef>
#include <initializer_list>
#include <memory>
#include <string>
#include <variant>
#include <vector>

namespace ppl {

using Symbol = std::string;

struct Expr {
    struct List { std::vector<Expr> items; };
    std::variant<double, Symbol, std::shared_ptr<List>> data;

    Expr() = default;
    explicit Expr(double d);
    explicit Expr(const Symbol& s);
    explicit Expr(Symbol&& s);
    explicit Expr(const List& l);
    explicit Expr(List&& l);
    explicit Expr(const std::vector<Expr>& items);
    explicit Expr(std::vector<Expr>&& items);
    Expr(std::initializer_list<Expr> init);

    bool is_number() const;
    bool is_symbol() const;
    bool is_list() const;

    double as_number() const;
    const Symbol& as_symbol() const;
    const List& as_list() const;
    List& as_list();

    size_t size() const;
    const Expr& operator[](size_t i) const;
    Expr& operator[](size_t i);
};

} // namespace ppl