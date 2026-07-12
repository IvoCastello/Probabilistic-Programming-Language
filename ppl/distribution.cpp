#include "distribution.h"

namespace ppl {

Normal::Normal(double mu_, double sigma_) : mu(mu_), sigma(sigma_) {}
double Normal::sample(std::mt19937& rng) const {
    return std::normal_distribution<double>{mu, sigma}(rng);
}
double Normal::log_prob(double x) const {
    static const double PI = 3.14159265358979323846;
    static const double LOG_2PI = std::log(2.0 * PI);
    double z = (x - mu) / sigma;
    return -0.5 * (z * z + LOG_2PI + 2.0 * std::log(sigma));
}

Bernoulli::Bernoulli(double p_) : p(p_) {}
double Bernoulli::sample(std::mt19937& rng) const {
    return std::bernoulli_distribution{p}(rng) ? 1.0 : 0.0;
}
double Bernoulli::log_prob(double x) const {
    int k = (x > 0.5) ? 1 : 0;
    return k * std::log(p) + (1 - k) * std::log(1.0 - p);
}

} // namespace ppl