#ifndef SORT_UTILS_H
#define SORT_UTILS_H

#include <vector>
#include "ThreadPool.h"

void merge(std::vector<int>& arr, int left, int mid, int right);
void parallel_merge_sort(ThreadPool& pool, std::vector<int>& arr, int left, int right, int depth = 0);
void merge_sort(std::vector<int>& arr, int left, int right);

#endif // SORT_UTILS_H
