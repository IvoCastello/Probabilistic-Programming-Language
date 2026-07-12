#pragma once
#include <cmath>
#include <random>

namespace ppl {

struct Normal {
    double mu, sigma;
    Normal(double mu_, double sigma_);
    Normal() = default;
    double sample(std::mt19937& rng) const;
    double log_prob(double x) const;
};

struct Bernoulli {
    double p;
    Bernoulli(double p_);
    Bernoulli() = default;
    double sample(std::mt19937& rng) const;
    double log_prob(double x) const;
};

} // namespace ppl