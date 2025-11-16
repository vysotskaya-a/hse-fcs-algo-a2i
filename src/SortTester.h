#ifndef SORT_TESTER_H
#define SORT_TESTER_H

#include <vector>
#include <chrono>
#include <algorithm>
#include <numeric>
#include <functional>
#include <stdexcept>

struct TimeResult {
    double mean_usec;
    double median_usec;
    std::vector<long long> runs_usec;
};

class SortTester {
public:
    explicit SortTester(int runs = 5) : runs_(runs) {
        if (runs_ <= 0) runs_ = 1;
    }

    // стандартный mergesort (рекурсивный, с выделением временного в merge)
    TimeResult time_merge_sort(const std::vector<int>& arr) const {
        auto fn = [](std::vector<int>& v) {
            if (!v.empty()) merge_sort_rec(v, 0, (int)v.size() - 1);
        };
        return measure_runs(arr, fn);
    }

    // гибридный: merge + insertion. threshold: если размер сегмента <= threshold -> insertion
    TimeResult time_merge_insertion_sort(const std::vector<int>& arr, int threshold) const {
        if (threshold < 1) throw std::invalid_argument("threshold must be >= 1");
        auto fn = [threshold](std::vector<int>& v) {
            if (!v.empty()) merge_sort_hybrid_rec(v, 0, (int)v.size() - 1, threshold);
        };
        return measure_runs(arr, fn);
    }

    int runs() const { return runs_; }

private:
    int runs_;

    static void insertion_sort(std::vector<int>& a, int l, int r) {
        for (int i = l + 1; i <= r; ++i) {
            int key = a[i];
            int j = i - 1;
            while (j >= l && a[j] > key) {
                a[j + 1] = a[j];
                --j;
            }
            a[j + 1] = key;
        }
    }

    static void merge_with_alloc(std::vector<int>& a, int l, int m, int r) {
        int n1 = m - l + 1;
        int n2 = r - m;
        std::vector<int> left(n1);
        std::vector<int> right(n2);
        for (int i = 0; i < n1; ++i) left[i] = a[l + i];
        for (int j = 0; j < n2; ++j) right[j] = a[m + 1 + j];

        int i = 0, j = 0, k = l;
        while (i < n1 && j < n2) {
            if (left[i] <= right[j]) a[k++] = left[i++];
            else a[k++] = right[j++];
        }
        while (i < n1) a[k++] = left[i++];
        while (j < n2) a[k++] = right[j++];
    }

    static void merge_sort_rec(std::vector<int>& a, int l, int r) {
        if (l >= r) return;
        int m = l + (r - l) / 2;
        merge_sort_rec(a, l, m);
        merge_sort_rec(a, m + 1, r);
        merge_with_alloc(a, l, m, r);
    }

    static void merge_sort_hybrid_rec(std::vector<int>& a, int l, int r, int threshold) {
        int len = r - l + 1;
        if (len <= 0) return;
        if (len <= threshold) {
            insertion_sort(a, l, r);
            return;
        }
        int m = l + (r - l) / 2;
        merge_sort_hybrid_rec(a, l, m, threshold);
        merge_sort_hybrid_rec(a, m + 1, r, threshold);
        merge_with_alloc(a, l, m, r);
    }

    TimeResult measure_runs(const std::vector<int>& arr, std::function<void(std::vector<int>&)> sort_fn) const {
        std::vector<long long> runs_us;
        runs_us.reserve(runs_);
        for (int i = 0; i < runs_; ++i) {
            std::vector<int> copy = arr;
            auto t1 = std::chrono::high_resolution_clock::now();
            sort_fn(copy);
            auto t2 = std::chrono::high_resolution_clock::now();
            long long usec = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
            runs_us.push_back(usec);
        }
        std::vector<long long> sorted = runs_us;
        std::sort(sorted.begin(), sorted.end());
        double mean = std::accumulate(sorted.begin(), sorted.end(), 0.0) / sorted.size();
        double median = (sorted.size() % 2 == 1) ? sorted[sorted.size()/2]
                       : (sorted[sorted.size()/2 - 1] + sorted[sorted.size()/2]) / 2.0;
        TimeResult res;
        res.mean_usec = mean;
        res.median_usec = median;
        res.runs_usec = runs_us;
        return res;
    }
};

#endif // SORT_TESTER_H
