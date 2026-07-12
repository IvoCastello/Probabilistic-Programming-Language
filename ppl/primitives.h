#pragma once
#include "value.h"
#include <map>

namespace ppl {

std::map<Symbol, Value> make_primitives();
bool is_primitive(const Symbol& s);
extern const std::map<Symbol, Value> PRIMITIVES;

} // namespace ppl