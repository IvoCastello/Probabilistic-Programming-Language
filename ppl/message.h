#pragma once
#include "address.h"
#include "value.h"
#include <variant>

namespace ppl {

struct DoneMsg    { Value value; };
struct SampleMsg  { Address addr; Value dist; };
struct ObserveMsg { Address addr; Value dist; double y; };

using Message = std::variant<DoneMsg, SampleMsg, ObserveMsg>;

} // namespace ppl