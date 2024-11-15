#include "sort_utils.h"
#include <cmath>
#include <algorithm>

void merge(std::vector<int>& arr, int left, int mid, int right) {
    int n1 = mid - left + 1;
    int n2 = right - mid;

    std::vector<int> leftArr(n1), rightArr(n2);

    for (int i = 0; i < n1; i++)
        leftArr[i] = arr[left + i];
    for (int i = 0; i < n2; i++)
        rightArr[i] = arr[mid + 1 + i];

    int i = 0, j = 0, k = left;
    while (i < n1 && j < n2) {
        if (leftArr[i] <= rightArr[j]) {
            arr[k] = leftArr[i];
            i++;
        } else {
            arr[k] = rightArr[j];
            j++;
        }
        k++;
    }

    while (i < n1) {
        arr[k] = leftArr[i];
        i++;
        k++;
    }

    while (j < n2) {
        arr[k] = rightArr[j];
        j++;
        k++;
    }
}

void parallel_merge_sort(ThreadPool& pool, std::vector<int>& arr, int left, int right, int depth) {
    if (left < right) {
        int mid = left + (right - left) / 2;

        if (depth < static_cast<int>(std::log2(pool.getNumThreads()))) {
            auto right_future = pool.enqueue(parallel_merge_sort, std::ref(pool), std::ref(arr), mid + 1, right, depth + 1);
            parallel_merge_sort(pool, arr, left, mid, depth + 1);
            right_future.get();
        } else {
            parallel_merge_sort(pool, arr, left, mid, depth + 1);
            parallel_merge_sort(pool, arr, mid + 1, right, depth + 1);
        }

        merge(arr, left, mid, right);
    }
}

void merge_sort(std::vector<int>& arr, int left, int right) {
    if (left >= right) {
        return;
    }
    int mid = left + (right - left) / 2;

    merge_sort(arr, left, mid);
    merge_sort(arr, mid + 1, right);
    merge(arr, left, mid, right);
}
