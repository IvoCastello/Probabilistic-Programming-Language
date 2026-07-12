#pragma once
#include <algorithm>
#include <string>
#include <variant>
#include <vector>

namespace ppl {

using Address = std::vector<std::variant<int, std::string>>;

struct VariantComparator {
    bool operator()(const std::variant<int, std::string>& a,
                    const std::variant<int, std::string>& b) const;
};

bool operator<(const Address& a, const Address& b);
bool operator==(const Address& a, const Address& b);

template<typename T>
Address operator+(Address a, T&& tail) {
    a.push_back(std::forward<T>(tail));
    return a;
}

} // namespace ppl