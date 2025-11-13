#pragma once
#include <vector>
#include <random>
#include <algorithm>
#include <stdexcept>
using namespace std;

class ArrayGenerator {
public:
    ArrayGenerator(int base_len, int rnd_max, unsigned seed = 123456)
        : base_len(base_len), rnd_max(rnd_max), rng(seed) {
        generate_all();
    }

    vector<int> get_prefix(const string& type, int n) {
        if (n > base_len) throw runtime_error("requested n > base_len");
        if (type == "random")  return vector<int>(random_base.begin(),  random_base.begin()  + n);
        if (type == "reverse") return vector<int>(reverse_base.begin(), reverse_base.begin() + n);
        if (type == "almost")  return vector<int>(almost_base.begin(),  almost_base.begin()  + n);
        throw runtime_error("unknown type");
    }

private:
    int base_len;
    int rnd_max;
    mt19937 rng;
    vector<int> random_base, reverse_base, almost_base;

    void generate_all() {
        uniform_int_distribution<int> dist(0, rnd_max);
        random_base.resize(base_len);
        for (int i = 0; i < base_len; ++i)
            random_base[i] = dist(rng);

        reverse_base = random_base;
        sort(reverse_base.begin(), reverse_base.end(), greater<int>());  // строго убывающий порядок

        almost_base = random_base;
        sort(almost_base.begin(), almost_base.end());  // сначала сортируем
        int swaps = base_len / 1000;                   // небольшое число случайных обменов
        uniform_int_distribution<int> pos(0, base_len - 1);
        for (int i = 0; i < swaps; ++i) {
            int a = pos(rng), b = pos(rng);
            swap(almost_base[a], almost_base[b]);
        }
    }
};
