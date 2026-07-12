#pragma once
#include "distribution.h"
#include "expr.h"
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <variant>
#include <vector>

namespace ppl {

struct Closure; // forward declaration

struct Value {
    using Primitive = std::function<Value(const std::vector<Value>&)>;
    using List = std::vector<Value>;

    std::variant<double, Symbol, std::shared_ptr<Closure>, Normal, Bernoulli, Primitive, List> data;

    Value() = default;
    Value(double d);
    explicit Value(const Symbol& s);
    explicit Value(Symbol&& s);
    Value(const std::shared_ptr<Closure>& c);
    Value(std::shared_ptr<Closure>&& c);
    Value(const Normal& n);
    Value(Normal&& n);
    Value(const Bernoulli& b);
    Value(Bernoulli&& b);
    Value(const Primitive& p);
    Value(Primitive&& p);
    explicit Value(const List& l);
    explicit Value(List&& l);

    double& as_number();
    const double& as_number() const;
    std::shared_ptr<Closure>& as_closure();
    const std::shared_ptr<Closure>& as_closure() const;
    Normal& as_normal();
    const Normal& as_normal() const;
    Bernoulli& as_bernoulli();
    const Bernoulli& as_bernoulli() const;
    Primitive& as_primitive();
    const Primitive& as_primitive() const;

    bool is_number() const;
    bool is_closure() const;
    bool is_primitive() const;
    bool is_normal() const;
    bool is_bernoulli() const;
};

using Env = std::map<Symbol, Value>;

struct Closure {
    std::vector<Symbol> params;
    std::vector<Expr> body;
    std::shared_ptr<Env> env;

    Closure(std::vector<Symbol> params, std::vector<Expr> body, std::shared_ptr<Env> env);
};

} // namespace ppl