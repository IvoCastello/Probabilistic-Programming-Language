#include "address.h"

namespace ppl {

bool VariantComparator::operator()(const std::variant<int, std::string>& a,
                                   const std::variant<int, std::string>& b) const {
    if (a.index() != b.index()) return a.index() < b.index();
    if (std::holds_alternative<int>(a))
        return std::get<int>(a) < std::get<int>(b);
    return std::get<std::string>(a) < std::get<std::string>(b);
}

bool operator<(const Address& a, const Address& b) {
    return std::lexicographical_compare(
        a.begin(), a.end(), b.begin(), b.end(), VariantComparator{});
}

bool operator==(const Address& a, const Address& b) {
    if (a.size() != b.size()) return false;
    for (size_t i = 0; i < a.size(); ++i) {
        if (a[i].index() != b[i].index()) return false;
        if (std::holds_alternative<int>(a[i])) {
            if (std::get<int>(a[i]) != std::get<int>(b[i])) return false;
        } else {
            if (std::get<std::string>(a[i]) != std::get<std::string>(b[i])) return false;
        }
    }
    return true;
}

} // namespace ppl