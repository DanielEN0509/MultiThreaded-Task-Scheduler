#include "ThreadPool.h"
#include <iostream>

ThreadPool::ThreadPool(size_t numThreads) {
    if (numThreads == 0) {
        numThreads = 4;
    }

    workers_.reserve(numThreads);
    for (size_t i = 0; i < numThreads; ++i) {
        workers_.emplace_back(&ThreadPool::workerLoop, this);
    }
}

ThreadPool::~ThreadPool() {
    {
        std::unique_lock<std::mutex> lock(queueMutex_);
        stop_.store(true);
    }

    // Wake all worker threads to process shutdown
    cv_.notify_all();

    // Safely join all threads to prevent std::terminate
    for (std::thread& worker : workers_) {
        if (worker.joinable()) {
            worker.join();
        }
    }
}

void ThreadPool::enqueue(std::function<void()> task) {
    {
        std::unique_lock<std::mutex> lock(queueMutex_);
        if (stop_.load()) {
            throw std::runtime_error("Cannot enqueue task on a stopped ThreadPool.");
        }
        taskQueue_.push(std::move(task));
    }

    // Notify a waiting thread that a new task is available
    cv_.notify_one();
}

void ThreadPool::workerLoop() {
    while (true) {
        std::function<void()> task;

        {
            std::unique_lock<std::mutex> lock(queueMutex_);
            cv_.wait(lock, [this]() {
                return stop_.load() || !taskQueue_.empty();
            });

            if (stop_.load() && taskQueue_.empty()) {
                return;
            }

            task = std::move(taskQueue_.front());
            taskQueue_.pop();
        }

        ++activeWorkers_;

        try {
            if (task) {
                task();
            }
        } catch (const std::exception& e) {
            std::cerr << "[ThreadPool Worker Error]: " << e.what() << std::endl;
        } catch (...) {
            std::cerr << "[ThreadPool Worker Error]: Unknown exception occurred." << std::endl;
        }

        --activeWorkers_;
    }
}