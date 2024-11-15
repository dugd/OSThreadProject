#include "ThreadPool.h"

#ifdef _WIN32
#include <windows.h>
#else
#include <pthread.h>
#endif

ThreadPool::ThreadPool(size_t numThreads) : numThreads(numThreads), stop(false) {
    for (size_t i = 0; i < numThreads; ++i) {
        workers.emplace_back([this] { this->worker(); });
    }
}

ThreadPool::~ThreadPool() {
    {
        std::unique_lock<std::mutex> lock(mtx);
        stop = true;
    }
    cv.notify_all();
    for (std::thread& worker : workers) {
        worker.join();
    }
}

void ThreadPool::worker() {
    while (true) {
        std::function<void()> task;
        {
            std::unique_lock<std::mutex> lock(mtx);
            cv.wait(lock, [this] { return stop || !tasks.empty(); });
            if (stop && tasks.empty()) return;
            task = std::move(tasks.front());
            tasks.pop();
        }
        task();
    }
}

size_t ThreadPool::getNumThreads() const {
    return numThreads;
}

void ThreadPool::setThreadPriority(int priority) {
    #ifdef _WIN32
    for (std::thread& worker : workers) {
        HANDLE handle = reinterpret_cast<HANDLE>(worker.native_handle());
        if (!SetThreadPriority(handle, priority)) {
            std::cerr << "Failed to set thread priority on Windows\n";
        }
    }
    #else
    std::cout << "Setting thread priority is not implemented on Linux\n";
    #endif
}