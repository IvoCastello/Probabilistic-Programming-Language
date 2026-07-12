#include "ppl/inference.h"
#include "ppl/vm.h"
#include <cassert>
#include <cmath>
#include <iostream>
#include <numeric>
#include <random>

using namespace ppl;
using namespace std;

int main() {
    try {
        string shift = "(let [make-shift (fn [mu] (fn [x] (+ x mu)))  f (make-shift 10)] (f 3))";
        mt19937 rng(0);
        auto [v, w] = run_lw(shift, rng);
        cout << "closure: (f 3) = " << v.as_number() << " (expect 13)" << endl;
        assert(v.as_number() == 13.0);

        string geom = "(defn geom [] (if (sample (bernoulli 0.3)) 0 (+ 1 (geom)))) (geom)";
        mt19937 rng2(1);
        vector<double> ks;
        for (int i = 0; i < 200000; ++i) {
            auto [v2, w2] = run_lw(geom, rng2);
            ks.push_back(v2.as_number());
        }
        double mean_geom = accumulate(ks.begin(), ks.end(), 0.0) / ks.size();
        cout << "geom mean = " << mean_geom << "   exact (1-p)/p = " << (0.7/0.3) << endl;

        string conj = "(let [mu (sample (normal 0 1))] (observe (normal mu 1) 2.3) mu)";
        mt19937 rng3(2);
        auto [vals, weights] = likelihood_weighting(conj, rng3, 100000);
        double lw_mean = 0.0;
        for (size_t i = 0; i < vals.size(); ++i) lw_mean += weights[i] * vals[i];
        cout << "LW   mean = " << lw_mean << "  (exact 1.150)" << endl;

        mt19937 rng4(0);
        auto chain = single_site_mh(conj, rng4, 60000, 3000);
        if (!chain.empty()) {
            double mh_mean = accumulate(chain.begin(), chain.end(), 0.0) / chain.size();
            double mh_var = 0.0;
            for (auto x : chain) mh_var += (x - mh_mean) * (x - mh_mean);
            mh_var /= chain.size();
            cout << "SSMH mean = " << mh_mean << "  std = " << sqrt(mh_var)
                 << "  (exact 1.150, " << sqrt(0.5) << ")" << endl;
        }

        string bits = "(let [b1 (if (sample (bernoulli 0.5)) 1 0) b2 (if (sample (bernoulli 0.5)) 1 0) b3 (if (sample (bernoulli 0.5)) 1 0) b4 (if (sample (bernoulli 0.5)) 1 0) b5 (if (sample (bernoulli 0.5)) 1 0) b6 (if (sample (bernoulli 0.5)) 1 0) b7 (if (sample (bernoulli 0.5)) 1 0) b8 (if (sample (bernoulli 0.5)) 1 0) total (+ b1 b2 b3 b4 b5 b6 b7 b8)] (observe (normal 7 2) total) total)";
        mt19937 rng5(1);
        auto chain3 = single_site_mh(bits, rng5, 40000, 3000);
        if (!chain3.empty()) {
            double mh_mean2 = accumulate(chain3.begin(), chain3.end(), 0.0) / chain3.size();
            cout << "bits SSMH mean = " << mh_mean2 << endl;
        }

        vector<mt19937> rngs_smc;
        for (int i = 0; i < 20000; ++i) rngs_smc.emplace_back(1000 + i);
        auto smc_res = run_smc(conj, rngs_smc, 20000);
        double smc_mean = accumulate(smc_res.begin(), smc_res.end(), 0.0) / smc_res.size();
        cout << "SMC  mean = " << smc_mean << endl;

        cout << "\nAll tests passed!" << endl;
    } catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }
    return 0;
}