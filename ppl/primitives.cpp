#include "primitives.h"

namespace ppl {

std::map<Symbol, Value> make_primitives() {
    std::map<Symbol, Value> P;
    P["+"] = Value::Primitive([](const std::vector<Value>& args) -> Value {
        double r = 0; for (auto& a : args) r += a.as_number(); return r;
    });
    P["-"] = Value::Primitive([](const std::vector<Value>& args) -> Value {
        if (args.empty()) return 0.0;
        double r = args[0].as_number();
        for (size_t i = 1; i < args.size(); ++i) r -= args[i].as_number();
        return r;
    });
    P["*"] = Value::Primitive([](const std::vector<Value>& args) -> Value {
        double r = 1; for (auto& a : args) r *= a.as_number(); return r;
    });
    P["/"] = Value::Primitive([](const std::vector<Value>& args) -> Value {
        if (args.empty()) return 1.0;
        double r = args[0].as_number();
        for (size_t i = 1; i < args.size(); ++i) r /= args[i].as_number();
        return r;
    });
    P["=="] = Value::Primitive([](const std::vector<Value>& args) -> Value {
        if (args.size() < 2) return 1.0;
        return args[0].as_number() == args[1].as_number() ? 1.0 : 0.0;
    });
    P["<"] = Value::Primitive([](const std::vector<Value>& args) -> Value {
        if (args.size() < 2) return 0.0;
        return args[0].as_number() < args[1].as_number() ? 1.0 : 0.0;
    });
    P[">"] = Value::Primitive([](const std::vector<Value>& args) -> Value {
        if (args.size() < 2) return 0.0;
        return args[0].as_number() > args[1].as_number() ? 1.0 : 0.0;
    });
    P["normal"] = Value::Primitive([](const std::vector<Value>& args) -> Value {
        return Value(Normal(args[0].as_number(), args[1].as_number()));
    });
    P["bernoulli"] = Value::Primitive([](const std::vector<Value>& args) -> Value {
        return Value(Bernoulli(args[0].as_number()));
    });
    return P;
}

const std::map<Symbol, Value> PRIMITIVES = make_primitives();

bool is_primitive(const Symbol& s) { return PRIMITIVES.count(s) > 0; }

}