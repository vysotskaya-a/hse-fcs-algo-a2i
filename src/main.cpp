#include <bits/stdc++.h>
#include "ArrayGenerator.h"
#include "SortTester.h"
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    const int BASE_LEN = 100000;
    const int RANDOM_MAX = 6000;
    const vector<int> THRESHOLDS = {5, 10, 20, 30, 50};

    ArrayGenerator gen(BASE_LEN, RANDOM_MAX);
    SortTester tester(gen, THRESHOLDS);

    auto results = tester.run_all();
    SortTester::export_csv(results, "../results/raw_results.csv", "../results/summary.csv");

    cerr << "Experiment finished. Results saved to /results.\n";
    return 0;
}
