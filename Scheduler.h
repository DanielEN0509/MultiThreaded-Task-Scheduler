#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "Task.h"
#include "ThreadPool.h"
#include <queue>
#include <unordered_map>
#include <memory>
#include <mutex>
#include <vector>

/**
 * @brief Custom comparator for heap ordering by Task priority.
 */
struct TaskComparator {
 bool operator()(const std::unique_ptr<Task>& lhs, const std::unique_ptr<Task>& rhs) const {
  return static_cast<int>(lhs->getPriority()) < static_cast<int>(rhs->getPriority());
 }
};

/**
 * @brief Manages task priority queuing and dispatches work to the ThreadPool.
 */
class Scheduler {
public:
 explicit Scheduler(std::shared_ptr<ThreadPool> threadPool);
 ~Scheduler() = default;

 Scheduler(const Scheduler&) = delete;
 Scheduler& operator=(const Scheduler&) = delete;

 /**
  * @brief Submits a task to the priority queue and tracks its result.
  */
 std::shared_ptr<TaskResult> submitTask(std::unique_ptr<Task> task);

 /**
  * @brief Dispatches the highest priority task to the ThreadPool.
  */
 bool dispatchNext();

 /**
  * @brief Dispatches all queued tasks to the ThreadPool.
  */
 size_t dispatchAll();

 /**
  * @brief Retrieves task execution result via std::weak_ptr lookup.
  */
 std::shared_ptr<TaskResult> getTaskResult(int taskId) const;

 // Telemetry getters
 size_t getPendingTaskCount() const;
 size_t getCompletedTaskCount() const;

private:
 std::shared_ptr<ThreadPool> threadPool_;

 // Priority heap storing move-only tasks
 std::priority_queue<std::unique_ptr<Task>, std::vector<std::unique_ptr<Task>>, TaskComparator> priorityQueue_;

 // Task result registry using std::weak_ptr to prevent cycle/ownership leaks
 mutable std::unordered_map<int, std::weak_ptr<TaskResult>> taskRegistry_;

 mutable std::mutex schedulerMutex_;
 size_t completedCount_{0};
};

#endif // SCHEDULER_H