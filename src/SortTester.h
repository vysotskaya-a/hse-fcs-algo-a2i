#pragma once
#include <bits/stdc++.h>
#include "sort_algorithms.h"
#include "ArrayGenerator.h"
using namespace std;

struct Measurement {
    string algorithm;
    string data_type;
    int threshold;
    int n;
    int run_index;
    double time_ms;
};

class SortTester {
public:
    SortTester(ArrayGenerator& gen,
               vector<int> thresholds,
               int min_n = 500,
               int max_n = 100000,
               int step = 100,
               int runs = 5)
        : gen(gen),
          thresholds(thresholds),
          MIN_N(min_n),
          MAX_N(max_n),
          STEP(step),
          NUM_RUNS(runs) {}

    vector<Measurement> run_all() {
        vector<Measurement> results;
        vector<string> types = {"random", "reverse", "almost"};
        for (int n = MIN_N; n <= MAX_N; n += STEP) {
            for (auto& t : types) {
                vector<int> base = gen.get_prefix(t, n);

                // стандартный merge sort
                for (int run = 0; run < NUM_RUNS; ++run) {
                    vector<int> arr = base, tmp(n);
                    auto t0 = chrono::high_resolution_clock::now();
                    merge_sort_std_rec(arr, tmp, 0, n - 1);
                    auto t1 = chrono::high_resolution_clock::now();
                    double ms = chrono::duration_cast<chrono::microseconds>(t1 - t0).count() / 1000.0;
                    results.push_back({"merge_std", t, -1, n, run, ms});
                }

                // гибридный merge sort
                for (int thr : thresholds) {
                    for (int run = 0; run < NUM_RUNS; ++run) {
                        vector<int> arr = base, tmp(n);
                        auto t0 = chrono::high_resolution_clock::now();
                        merge_sort_hybrid_rec(arr, tmp, 0, n - 1, thr);
                        auto t1 = chrono::high_resolution_clock::now();
                        double ms = chrono::duration_cast<chrono::microseconds>(t1 - t0).count() / 1000.0;
                        results.push_back({"merge_hybrid", t, thr, n, run, ms});
                    }
                }
            }
            if (n % 5000 == 0)
                cerr << "Progress: n=" << n << endl;
        }
        return results;
    }

    static void export_csv(const vector<Measurement>& data,
                           string raw_file,
                           string summary_file) {
        ofstream raw(raw_file);
        raw << "algorithm,data_type,threshold,n,run_index,time_ms\n";
        for (auto& m : data)
            raw << m.algorithm << "," << m.data_type << "," << m.threshold << ","
                << m.n << "," << m.run_index << "," << m.time_ms << "\n";
        raw.close();

        // сводка
        map<tuple<string,string,int,int>, vector<double>> groups;
        for (auto& m : data)
            groups[{m.algorithm,m.data_type,m.threshold,m.n}].push_back(m.time_ms);

        ofstream summary(summary_file);
        summary << "algorithm,data_type,threshold,n,mean_ms,median_ms,stddev_ms\n";
        for (auto& [key, vals] : groups) {
            auto [alg, dt, thr, n] = key;
            sort(vals.begin(), vals.end());
            double mean = accumulate(vals.begin(), vals.end(), 0.0) / vals.size();
            double median = vals[vals.size()/2];
            double var = 0; for (auto v : vals) var += (v-mean)*(v-mean);
            var /= vals.size();
            summary << alg << "," << dt << "," << thr << "," << n << ","
                    << mean << "," << median << "," << sqrt(var) << "\n";
        }
        summary.close();
    }

private:
    ArrayGenerator& gen;
    vector<int> thresholds;
    int MIN_N, MAX_N, STEP, NUM_RUNS;
};
