#include "inference.h"
#include "vm.h"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <random>
#include <set>

namespace ppl {

Value sample_dist(const Value& dist, std::mt19937& rng) {
    if (dist.is_normal()) return Value(dist.as_normal().sample(rng));
    if (dist.is_bernoulli()) return Value(dist.as_bernoulli().sample(rng));
    throw std::runtime_error("sample_dist: unknown distribution");
}

double log_prob_dist(const Value& dist, double x) {
    if (dist.is_normal()) return dist.as_normal().log_prob(x);
    if (dist.is_bernoulli()) return dist.as_bernoulli().log_prob(x);
    throw std::runtime_error("log_prob_dist: unknown distribution");
}

std::vector<double> softmax(const std::vector<double>& log_w) {
    double mx = *std::max_element(log_w.begin(), log_w.end());
    std::vector<double> w(log_w.size());
    double sum = 0.0;
    for (size_t i = 0; i < log_w.size(); ++i) {
        w[i] = std::exp(log_w[i] - mx);
        sum += w[i];
    }
    for (auto& x : w) x /= sum;
    return w;
}

static Message advance(Machine& m) {
    auto msg = resume(m);
    while (std::holds_alternative<SampleMsg>(msg)) {
        auto& s = std::get<SampleMsg>(msg);
        send(m, sample_dist(s.dist, *m.rng));
        msg = resume(m);
    }
    return msg;
}

std::pair<Value, double> run_lw(const std::string& program, std::mt19937& rng) {
    auto m = initial_machine(program, rng);
    while (true) {
        auto msg = resume(m);
        if (std::holds_alternative<DoneMsg>(msg)) {
            return {std::get<DoneMsg>(msg).value, m.log_w};
        }
        else if (auto* s = std::get_if<SampleMsg>(&msg)) {
            send(m, sample_dist(s->dist, *m.rng));
        }
        else if (auto* o = std::get_if<ObserveMsg>(&msg)) {
            m.log_w += log_prob_dist(o->dist, o->y);
            send(m, Value(o->y));
        }
    }
}

std::pair<std::vector<double>, std::vector<double>> likelihood_weighting(const std::string& prog, std::mt19937& rng, int N) {
    std::vector<double> values, log_ws;
    for (int i = 0; i < N; ++i) {
        auto [v, lw] = run_lw(prog, rng);
        values.push_back(v.as_number());
        log_ws.push_back(lw);
    }
    auto w = softmax(log_ws);
    return {std::move(values), std::move(w)};
}

std::vector<double> run_smc(const std::string& prog, std::vector<std::mt19937>& rngs, int N) {
    std::vector<Machine> particles;
    for (int i = 0; i < N; ++i) particles.push_back(initial_machine(prog, rngs[i]));

    while (true) {
        std::vector<std::pair<Machine*, Message>> paused;
        for (auto& p : particles) {
            auto msg = advance(p);
            paused.emplace_back(&p, msg);
        }

        bool all_done = true, all_obs = true;
        for (auto& [_, msg] : paused) {
            all_done &= std::holds_alternative<DoneMsg>(msg);
            all_obs  &= std::holds_alternative<ObserveMsg>(msg);
        }
        if (all_done) {
            std::vector<double> res;
            for (auto& [_, msg] : paused) res.push_back(std::get<DoneMsg>(msg).value.as_number());
            return res;
        }

        std::vector<double> log_inc;
        for (auto& [mptr, msg] : paused) {
            auto& o = std::get<ObserveMsg>(msg);
            double lp = log_prob_dist(o.dist, o.y);
            mptr->log_w += lp;
            log_inc.push_back(lp);
            send(*mptr, Value(o.y));
        }

        auto probs = softmax(log_inc);
        std::discrete_distribution<int> dist(probs.begin(), probs.end());
        std::vector<Machine> new_particles;
        for (int j = 0; j < N; ++j) {
            int i = dist(rngs[0]);
            new_particles.push_back(paused[i].first->fork(&rngs[j]));
        }
        particles = std::move(new_particles);
    }
}

std::tuple<Value, Trace, LogProbs, LogProbs>
run_mh(const std::string& prog, std::mt19937& rng,
       const std::optional<Address>& x0,
       const Trace& cache) {

    auto m = initial_machine(prog, rng);
    Trace X; LogProbs S, O;

    while (true) {
        auto msg = resume(m);
        if (auto* s = std::get_if<SampleMsg>(&msg)) {
            Value x;
            if (cache.count(s->addr) && (!x0 || s->addr != *x0)) {
                x = cache.at(s->addr);
            } else {
                x = sample_dist(s->dist, *m.rng);
            }
            X[s->addr] = x;
            S[s->addr] = log_prob_dist(s->dist, x.as_number());
            send(m, x);
        }
        else if (auto* o = std::get_if<ObserveMsg>(&msg)) {
            O[o->addr] = log_prob_dist(o->dist, o->y);
            send(m, Value(o->y));
        }
        else if (auto* d = std::get_if<DoneMsg>(&msg)) {
            return {d->value, X, S, O};
        }
    }
}

double mh_log_alpha(const Trace& X, const Trace& X2,
                    const LogProbs& S, const LogProbs& S2,
                    const LogProbs& O, const LogProbs& O2,
                    const Address& a0) {
    std::set<Address> fwd{a0}, rev{a0};
    for (auto& [k,_] : X2) if (!X.count(k)) fwd.insert(k);
    for (auto& [k,_] : X)  if (!X2.count(k)) rev.insert(k);

    double num = 0.0, den = 0.0;
    for (auto& [k,p] : S2) if (!fwd.count(k)) num += p;
    for (auto& [k,p] : O2) num += p;
    for (auto& [k,p] : S)  if (!rev.count(k)) den += p;
    for (auto& [k,p] : O)  den += p;

    return (std::log((double)X.size()) - std::log((double)X2.size())) + (num - den);
}

std::vector<double> single_site_mh(const std::string& prog, std::mt19937& rng,
                                   int steps, int warmup) {
    auto [value, X, S, O] = run_mh(prog, rng, std::nullopt, {});
    std::vector<double> chain;

    for (int i = 0; i < steps + warmup; ++i) {
        std::vector<Address> addrs;
        for (auto& [k,_] : X) addrs.push_back(k);
        if (addrs.empty()) break;

        std::uniform_int_distribution<std::size_t> idx(0, addrs.size() - 1);
        Address a0 = addrs[idx(rng)];

        auto [v2, X2, S2, O2] = run_mh(prog, rng, a0, X);
        double la = mh_log_alpha(X, X2, S, S2, O, O2, a0);

        std::uniform_real_distribution<double> unif(0.0, 1.0);
        if (std::log(unif(rng)) < la) {
            value = v2; X = X2; S = S2; O = O2;
        }
        if (i >= warmup) chain.push_back(value.as_number());
    }
    return chain;
}

} // namespace ppl