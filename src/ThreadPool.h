#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <functional>
#include <future>

#ifdef _WIN32 
#  ifdef THREADLIB_EXPORTS
#    define THREADLIB_API __declspec(dllexport)
#  else
#    define THREADLIB_API __declspec(dllimport)
#  endif
#else
#  define THREADLIB_API
#endif

class THREADLIB_API ThreadPool {
public:
    explicit ThreadPool(size_t numThreads);
    ~ThreadPool();

    template<class F, class... Args>
    auto enqueue(F&& f, Args&&... args) -> std::future<typename std::result_of<F(Args...)>::type>;
    size_t getNumThreads() const;
    void setThreadPriority(int priority);

private:
    size_t numThreads;
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;
    std::mutex mtx;
    std::condition_variable cv;
    bool stop;
    void worker();
};

template<class F, class... Args>
auto ThreadPool::enqueue(F&& f, Args&&... args) -> std::future<typename std::result_of<F(Args...)>::type> {
    using return_type = typename std::result_of<F(Args...)>::type;
    auto task = std::make_shared<std::packaged_task<return_type()>>(std::bind(std::forward<F>(f), std::forward<Args>(args)...));
    std::future<return_type> res = task->get_future();
    {
        std::unique_lock<std::mutex> lock(mtx);
        tasks.emplace([task]() { (*task)(); });
    }
    cv.notify_one();
    return res;
}

#endif // THREADPOOL_H
