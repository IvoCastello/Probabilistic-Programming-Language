#include "value.h"

namespace ppl {

Value::Value(double d) : data(d) {}
Value::Value(const Symbol& s) : data(s) {}
Value::Value(Symbol&& s) : data(std::move(s)) {}
Value::Value(const std::shared_ptr<Closure>& c) : data(c) {}
Value::Value(std::shared_ptr<Closure>&& c) : data(std::move(c)) {}
Value::Value(const Normal& n) : data(n) {}
Value::Value(Normal&& n) : data(std::move(n)) {}
Value::Value(const Bernoulli& b) : data(b) {}
Value::Value(Bernoulli&& b) : data(std::move(b)) {}
Value::Value(const Primitive& p) : data(p) {}
Value::Value(Primitive&& p) : data(std::move(p)) {}
Value::Value(const List& l) : data(l) {}
Value::Value(List&& l) : data(std::move(l)) {}

double& Value::as_number() { return std::get<double>(data); }
const double& Value::as_number() const { return std::get<double>(data); }

std::shared_ptr<Closure>& Value::as_closure() { return std::get<std::shared_ptr<Closure>>(data); }
const std::shared_ptr<Closure>& Value::as_closure() const { return std::get<std::shared_ptr<Closure>>(data); }

Normal& Value::as_normal() { return std::get<Normal>(data); }
const Normal& Value::as_normal() const { return std::get<Normal>(data); }

Bernoulli& Value::as_bernoulli() { return std::get<Bernoulli>(data); }
const Bernoulli& Value::as_bernoulli() const { return std::get<Bernoulli>(data); }

Value::Primitive& Value::as_primitive() { return std::get<Primitive>(data); }
const Value::Primitive& Value::as_primitive() const { return std::get<Primitive>(data); }

bool Value::is_number() const { return std::holds_alternative<double>(data); }
bool Value::is_closure() const { return std::holds_alternative<std::shared_ptr<Closure>>(data); }
bool Value::is_primitive() const { return std::holds_alternative<Primitive>(data); }
bool Value::is_normal() const { return std::holds_alternative<Normal>(data); }
bool Value::is_bernoulli() const { return std::holds_alternative<Bernoulli>(data); }

Closure::Closure(std::vector<Symbol> params_, std::vector<Expr> body_, std::shared_ptr<Env> env_)
    : params(std::move(params_)), body(std::move(body_)), env(std::move(env_)) {}

} // namespace ppl