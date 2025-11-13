#pragma once
#include <vector>
using namespace std;

// INSERTION SORT
inline void insertion_sort(vector<int>& arr, int l, int r) {
    for (int i = l + 1; i <= r; ++i) {
        int key = arr[i];
        int j = i - 1;
        while (j >= l && arr[j] > key) {
            arr[j + 1] = arr[j];
            --j;
        }
        arr[j + 1] = key;
    }
}

// MERG 
inline void merge_kernel(vector<int>& arr, vector<int>& tmp, int l, int mid, int r) {
    int i = l, j = mid + 1, k = l;
    while (i <= mid && j <= r) {
        if (arr[i] <= arr[j]) tmp[k++] = arr[i++];
        else tmp[k++] = arr[j++];
    }
    while (i <= mid) tmp[k++] = arr[i++];
    while (j <= r) tmp[k++] = arr[j++];
    for (int p = l; p <= r; ++p) arr[p] = tmp[p];
}

// STANDARD MERGE SORT
inline void merge_sort_std_rec(vector<int>& arr, vector<int>& tmp, int l, int r) {
    if (l >= r) return;
    int mid = l + (r - l) / 2;
    merge_sort_std_rec(arr, tmp, l, mid);
    merge_sort_std_rec(arr, tmp, mid + 1, r);
    merge_kernel(arr, tmp, l, mid, r);
}

// HYBRID MERGE + INSERTION
inline void merge_sort_hybrid_rec(vector<int>& arr, vector<int>& tmp, int l, int r, int threshold) {
    if (r - l + 1 <= threshold) {
        insertion_sort(arr, l, r);
        return;
    }
    int mid = l + (r - l) / 2;
    merge_sort_hybrid_rec(arr, tmp, l, mid, threshold);
    merge_sort_hybrid_rec(arr, tmp, mid + 1, r, threshold);
    merge_kernel(arr, tmp, l, mid, r);
}
