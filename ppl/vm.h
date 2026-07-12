#pragma once
#include "expr.h"
#include "machine.h"
#include "message.h"

namespace ppl {

Machine initial_machine(const std::string& program, std::mt19937& rng);

void push_body(std::vector<Instruction>& C, const std::vector<Expr>& body,
               const Env& env, const Address& addr);

Message resume(Machine& m);

inline void send(Machine& m, const Value& v) { m.V.push_back(v); }

} // namespace ppl