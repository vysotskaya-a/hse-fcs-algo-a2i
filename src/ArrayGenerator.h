#ifndef ARRAY_GENERATOR_H
#define ARRAY_GENERATOR_H

#include <vector>
#include <random>
#include <algorithm>
#include <stdexcept>

enum class ArrayType {
    RANDOM,
    REVERSE_SORTED,
    ALMOST_SORTED
};

class ArrayGenerator {
public:
    ArrayGenerator(int max_size = 100000, int rnd_min = 0, int rnd_max = 6000, unsigned seed = 12345)
        : max_size_(max_size), rnd_min_(rnd_min), rnd_max_(rnd_max), seed_(seed), rng_(seed_)
    {
        generate_max_arrays();
    }

    // Возвращает префикс нужного размера из заранее сгенерированного массива соответствующего типа
    std::vector<int> get_prefix(ArrayType type, int size) const {
        if (size <= 0 || size > max_size_) throw std::out_of_range("requested size out of range");
        switch (type) {
            case ArrayType::RANDOM:
                return std::vector<int>(arr_random_.begin(), arr_random_.begin() + size);
            case ArrayType::REVERSE_SORTED:
                return std::vector<int>(arr_reverse_sorted_.begin(), arr_reverse_sorted_.begin() + size);
            case ArrayType::ALMOST_SORTED:
                return std::vector<int>(arr_almost_sorted_.begin(), arr_almost_sorted_.begin() + size);
            default:
                return {};
        }
    }

    int max_size() const { return max_size_; }
    int rnd_min() const { return rnd_min_; }
    int rnd_max() const { return rnd_max_; }

private:
    int max_size_;
    int rnd_min_;
    int rnd_max_;
    unsigned seed_;
    mutable std::mt19937 rng_;

    std::vector<int> arr_random_;
    std::vector<int> arr_reverse_sorted_;
    std::vector<int> arr_almost_sorted_;

    int almost_swaps() const {
        int s1 = 100;
        int s2 = std::max(1, max_size_ / 1000); // 0.1% of n
        return std::min(s1, s2);
    }

    void generate_max_arrays() {
        arr_random_.resize(max_size_);
        std::uniform_int_distribution<int> dist(rnd_min_, rnd_max_);
        for (int i = 0; i < max_size_; ++i) arr_random_[i] = dist(rng_);

        // Reverse sorted (non-increasing)
        arr_reverse_sorted_.resize(max_size_);
        // Construct some values and sort descending
        std::vector<int> tmp(max_size_);
        for (int i = 0; i < max_size_; ++i) tmp[i] = rnd_min_ + (i % (rnd_max_ - rnd_min_ + 1));
        std::sort(tmp.begin(), tmp.end(), std::greater<int>());
        arr_reverse_sorted_ = tmp;

        // Almost sorted: sort random array and swap a few pairs
        std::vector<int> sorted = arr_random_;
        std::sort(sorted.begin(), sorted.end()); // non-decreasing
        arr_almost_sorted_ = sorted;

        int swaps = almost_swaps();
        std::uniform_int_distribution<int> pos_dist(0, max_size_ - 1);
        for (int k = 0; k < swaps; ++k) {
            int i = pos_dist(rng_);
            int j = pos_dist(rng_);
            std::swap(arr_almost_sorted_[i], arr_almost_sorted_[j]);
        }
    }
};

#endif // ARRAY_GENERATOR_H
