#pragma once
#include "address.h"
#include "machine.h"
#include "value.h"
#include <map>
#include <optional>
#include <random>
#include <vector>

namespace ppl {

using Trace = std::map<Address, Value>;
using LogProbs = std::map<Address, double>;

Value sample_dist(const Value& dist, std::mt19937& rng);
double log_prob_dist(const Value& dist, double x);
std::vector<double> softmax(const std::vector<double>& log_w);

std::pair<Value, double> run_lw(const std::string& program, std::mt19937& rng);
std::pair<std::vector<double>, std::vector<double>> likelihood_weighting(const std::string& prog, std::mt19937& rng, int N);

std::vector<double> run_smc(const std::string& prog, std::vector<std::mt19937>& rngs, int N);

std::tuple<Value, Trace, LogProbs, LogProbs>
run_mh(const std::string& prog, std::mt19937& rng,
       const std::optional<Address>& x0,
       const Trace& cache);

double mh_log_alpha(const Trace& X, const Trace& X2,
                    const LogProbs& S, const LogProbs& S2,
                    const LogProbs& O, const LogProbs& O2,
                    const Address& a0);

std::vector<double> single_site_mh(const std::string& prog, std::mt19937& rng,
                                   int steps, int warmup = 2000);

} // namespace ppl