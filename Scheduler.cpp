#include "Scheduler.h"
#include <iostream>

Scheduler::Scheduler(std::shared_ptr<ThreadPool> threadPool)
    : threadPool_(threadPool) {
    if (!threadPool_) {
        throw std::invalid_argument("Scheduler requires a valid ThreadPool pointer.");
    }
}

std::shared_ptr<TaskResult> Scheduler::submitTask(std::unique_ptr<Task> task) {
    if (!task) {
        return nullptr;
    }

    std::lock_guard<std::mutex> lock(schedulerMutex_);

    int id = task->getId();
    std::shared_ptr<TaskResult> resultHandle = task->getResult();

    // Store weak_ptr to observe execution without holding ownership
    taskRegistry_[id] = resultHandle;
    priorityQueue_.push(std::move(task));

    return resultHandle;
}

bool Scheduler::dispatchNext() {
    std::unique_ptr<Task> taskToExecute;

    {
        std::lock_guard<std::mutex> lock(schedulerMutex_);
        if (priorityQueue_.empty()) {
            return false;
        }

        taskToExecute = std::move(const_cast<std::unique_ptr<Task>&>(priorityQueue_.top()));
        priorityQueue_.pop();
    }

    // Convert to shared_ptr ensuring lambda copy-constructibility for std::function
    std::shared_ptr<Task> sharedTask = std::move(taskToExecute);

    threadPool_->enqueue([this, task = sharedTask]() {
        if (task) {
            task->execute();
            std::lock_guard<std::mutex> lock(schedulerMutex_);
            ++completedCount_;
        }
    });

    return true;
}

size_t Scheduler::dispatchAll() {
    size_t count = 0;
    while (dispatchNext()) {
        ++count;
    }
    return count;
}

std::shared_ptr<TaskResult> Scheduler::getTaskResult(int taskId) const {
    std::lock_guard<std::mutex> lock(schedulerMutex_);

    auto it = taskRegistry_.find(taskId);
    if (it != taskRegistry_.end()) {
        if (std::shared_ptr<TaskResult> sharedRes = it->second.lock()) {
            return sharedRes;
        }
    }
    return nullptr;
}

size_t Scheduler::getPendingTaskCount() const {
    std::lock_guard<std::mutex> lock(schedulerMutex_);
    return priorityQueue_.size();
}

size_t Scheduler::getCompletedTaskCount() const {
    std::lock_guard<std::mutex> lock(schedulerMutex_);
    return completedCount_;
}