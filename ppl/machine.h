#pragma once
#include "instruction.h"
#include "value.h"
#include <random>
#include <vector>

namespace ppl {

class Machine {
public:
    std::vector<Instruction> C;
    std::vector<Value> V;
    Env env;
    std::mt19937* rng;
    double log_w;

    Machine(std::vector<Instruction> C_ = {},
            std::vector<Value> V_ = {},
            Env env_ = {},
            std::mt19937* rng_ = nullptr,
            double log_w_ = 0.0);

    Machine fork(std::mt19937* withRNG) const;
    Machine fork() const;
};

} // namespace ppl