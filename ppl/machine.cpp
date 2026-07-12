#include "machine.h"

namespace ppl {

Machine::Machine(std::vector<Instruction> C_, std::vector<Value> V_, Env env_,
                 std::mt19937* rng_, double log_w_)
    : C(std::move(C_)), V(std::move(V_)), env(std::move(env_)), rng(rng_), log_w(log_w_) {}

Machine Machine::fork(std::mt19937* withRNG) const {
    return Machine(C, V, env, withRNG, log_w);
}
Machine Machine::fork() const { return fork(rng); }

} // namespace ppl