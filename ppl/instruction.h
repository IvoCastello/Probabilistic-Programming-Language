#pragma once
#include "address.h"
#include "expr.h"
#include "value.h"
#include <variant>
#include <vector>

namespace ppl {

struct Instruction {
    struct Ev {
        Expr expr;
        Env env;
        Address addr;
    };
    struct LetK {
        std::vector<Expr> binds;
        std::size_t i;
        std::vector<Expr> body;
        Env env;
        Address addr;
    };
    struct IfK {
        Expr then_branch;
        Expr else_branch;
        Env env;
        Address addr;
    };
    struct Discard {};
    struct CallK {
        std::size_t n;
        Address addr;
    };
    struct SampleK {
        Address addr;
    };
    struct ObserveK {
        Address addr;
    };

    std::variant<Ev, LetK, IfK, Discard, CallK, SampleK, ObserveK> data;
};

} // namespace ppl