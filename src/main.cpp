#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <cstdlib>
#include <cstring>

#include "ThreadPool.h"
#include "MappingFile.h"
#include "sort_utils.h"

int main(int argc, char* argv[]) {
    int array_size = 1000000;
    int num_threads = 8;

    if (argc > 1) {
        array_size = std::atoi(argv[1]);
    }
    if (argc > 2) {
        num_threads = std::atoi(argv[2]);
    }

    MappingFile mapping_file("array_data.bin", array_size);
    int* arr_m = mapping_file.data();
    std::vector<int> arr(arr_m, arr_m + mapping_file.size());

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, array_size);
    for (int& x : arr) {
        x = dist(gen);
    }

    MappingFile mapping_file_copy("array_data_copy.bin", array_size);
    int* arr_m_copy = mapping_file_copy.data();

    std::memcpy(arr_m_copy, arr_m, array_size * sizeof(int));

    std::vector<int> arr_copy(arr_m_copy, arr_m_copy + mapping_file_copy.size());

    ThreadPool pool(num_threads);
    pool.setThreadPriority(2); // THREAD_PRIORITY_HIGHEST

    auto start = std::chrono::high_resolution_clock::now();
    auto sort_future = pool.enqueue(parallel_merge_sort, std::ref(pool), std::ref(arr), 0, arr.size() - 1, 0);
    sort_future.get();
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> parallel_duration = end - start;
    std::cout << "Parallel merge sort: " << parallel_duration.count() << " seconds\n";

    start = std::chrono::high_resolution_clock::now();
    merge_sort(arr_copy, 0, arr_copy.size() - 1);
    end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> normal_duration = end - start;
    std::cout << "Ordinal merge sort: " << normal_duration.count() << " seconds\n";

    return 0;
}
