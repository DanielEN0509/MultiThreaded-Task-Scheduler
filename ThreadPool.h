#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include "Task.h"
#include <vector>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <functional>
#include <memory>

/**
 * @brief Manages a fixed pool of worker threads for concurrent task execution.
 */
class ThreadPool {
public:
 explicit ThreadPool(size_t numThreads = std::thread::hardware_concurrency());
 ~ThreadPool();

 // Enforce non-copyable and non-movable semantics for thread safety
 ThreadPool(const ThreadPool&) = delete;
 ThreadPool& operator=(const ThreadPool&) = delete;
 ThreadPool(ThreadPool&&) = delete;
 ThreadPool& operator=(ThreadPool&&) = delete;

 /**
  * @brief Enqueues a generic callable task into the work queue.
  */
 void enqueue(std::function<void()> task);

 // Telemetry getters
 size_t getThreadCount() const { return workers_.size(); }
 size_t getActiveWorkerCount() const { return activeWorkers_.load(); }
 bool isStopped() const { return stop_.load(); }

private:
 /**
  * @brief Worker thread loop for fetching and executing enqueued tasks.
  */
 void workerLoop();

 std::vector<std::thread> workers_;
 std::queue<std::function<void()>> taskQueue_;

 // Synchronization primitives
 mutable std::mutex queueMutex_;
 std::condition_variable cv_;

 // Atomic flags for status monitoring and shutdown management
 std::atomic<bool> stop_{false};
 std::atomic<size_t> activeWorkers_{0};
};

#endif // THREAD_POOL_H