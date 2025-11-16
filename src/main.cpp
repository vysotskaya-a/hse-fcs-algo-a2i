#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <chrono>
#include <iomanip>

#include "ArrayGenerator.h"
#include "SortTester.h"

struct Config {
    int min_n = 500;
    int max_n = 100000;
    int step = 100;
    int runs = 5;
    std::vector<int> thresholds = {5, 10, 20, 30, 50};
    std::string out_csv = "results.csv";
    unsigned seed = (unsigned)std::chrono::high_resolution_clock::now().time_since_epoch().count();
    bool run_standard = true;
    bool run_hybrid = true;
};

static std::vector<int> parse_int_list(const std::string& s) {
    std::vector<int> res;
    std::stringstream ss(s);
    int x;
    char c;
    while (ss >> x) {
        res.push_back(x);
        if (!(ss >> c)) break;
    }
    return res;
}

void print_usage(const char* prog) {
    std::cout << "Usage: " << prog << " [options]\n"
              << "Options:\n"
              << "  --min N            min array size (default 500)\n"
              << "  --max N            max array size (default 100000)\n"
              << "  --step N           step (default 100)\n"
              << "  --runs N           measurement runs per experiment (default 5)\n"
              << "  --thresholds a,b,c thresholds for hybrid (default 5,10,20,30,50)\n"
              << "  --out filename     output csv (default results.csv)\n"
              << "  --seed S           RNG seed\n"
              << "  --no-standard      do not run standard mergesort\n"
              << "  --no-hybrid        do not run hybrid mergesort\n"
              << "  --help             this message\n";
}

int main(int argc, char** argv) {
    Config cfg;
    for (int i = 1; i < argc; ++i) {
        std::string a = argv[i];
        if (a == "--min" && i+1 < argc) cfg.min_n = std::stoi(argv[++i]);
        else if (a == "--max" && i+1 < argc) cfg.max_n = std::stoi(argv[++i]);
        else if (a == "--step" && i+1 < argc) cfg.step = std::stoi(argv[++i]);
        else if (a == "--runs" && i+1 < argc) cfg.runs = std::stoi(argv[++i]);
        else if (a == "--thresholds" && i+1 < argc) cfg.thresholds = parse_int_list(argv[++i]);
        else if (a == "--out" && i+1 < argc) cfg.out_csv = argv[++i];
        else if (a == "--seed" && i+1 < argc) cfg.seed = (unsigned)std::stoul(argv[++i]);
        else if (a == "--no-standard") cfg.run_standard = false;
        else if (a == "--no-hybrid") cfg.run_hybrid = false;
        else if (a == "--help") { print_usage(argv[0]); return 0; }
        else { std::cerr << "Unknown arg: " << a << "\n"; print_usage(argv[0]); return 1; }
    }

    if (cfg.min_n < 1 || cfg.max_n < cfg.min_n || cfg.step < 1) {
        std::cerr << "Bad size range\n"; return 2;
    }

    std::cout << "Seed: " << cfg.seed << "\n";
    ArrayGenerator gen(100000, 0, 6000, cfg.seed);
    SortTester tester(cfg.runs);

    std::ofstream fout(cfg.out_csv);
    if (!fout) { std::cerr << "Cannot open " << cfg.out_csv << " for writing\n"; return 3; }

    fout << "array_type,n,algorithm,threshold,mean_usec,median_usec,runs_usec\n";
    std::vector<ArrayType> types = {ArrayType::RANDOM, ArrayType::REVERSE_SORTED, ArrayType::ALMOST_SORTED};
    auto type_name = [](ArrayType t)->std::string{
        switch (t) {
            case ArrayType::RANDOM: return "random";
            case ArrayType::REVERSE_SORTED: return "reverse_sorted";
            case ArrayType::ALMOST_SORTED: return "almost_sorted";
        }
        return "unknown";
    };

    int cnt = 0;
    int total = ((cfg.max_n - cfg.min_n) / cfg.step + 1) * (int)types.size() * ( (cfg.run_standard?1:0) + (cfg.run_hybrid? (int)cfg.thresholds.size() : 0) );

    for (auto t : types) {
        for (int n = cfg.min_n; n <= cfg.max_n; n += cfg.step) {
            auto arr = gen.get_prefix(t, n);

            if (cfg.run_standard) {
                auto res = tester.time_merge_sort(arr);
                fout << type_name(t) << "," << n << ",standard,NA," << std::fixed << res.mean_usec << "," << res.median_usec << ",\"";
                for (size_t i = 0; i < res.runs_usec.size(); ++i) { if (i) fout << ";"; fout << res.runs_usec[i]; }
                fout << "\"\n";
                ++cnt;
                if (cnt % 50 == 0) std::cout << "Progress: " << cnt << " / " << total << "\n";
            }

            if (cfg.run_hybrid) {
                for (int thr : cfg.thresholds) {
                    auto res = tester.time_merge_insertion_sort(arr, thr);
                    fout << type_name(t) << "," << n << ",hybrid," << thr << "," << std::fixed << res.mean_usec << "," << res.median_usec << ",\"";
                    for (size_t i = 0; i < res.runs_usec.size(); ++i) { if (i) fout << ";"; fout << res.runs_usec[i]; }
                    fout << "\"\n";
                    ++cnt;
                    if (cnt % 50 == 0) std::cout << "Progress: " << cnt << " / " << total << "\n";
                }
            }

        }
    }

    fout.close();
    std::cout << "Done. Results written to " << cfg.out_csv << "\n";
    return 0;
}
