#ifndef TASK_H
#define TASK_H

#include <string>
#include <memory>
#include <chrono>
#include <iostream>

enum class Priority {
    LOW = 0,
    MEDIUM = 1,
    HIGH = 2,
    CRITICAL = 3
};

enum class TaskType {
    COMPUTE,
    FILE_IO,
    NETWORK
};

/**
 * @brief Container for task execution results, shared across threads.
 */
struct TaskResult {
    int taskId;
    bool success;
    std::string outputMessage;
    long long executionTimeMs;

    TaskResult(int id)
        : taskId(id), success(false), outputMessage(""), executionTimeMs(0) {}
};

/**
 * @brief Abstract base class representing an executable task.
 */
class Task {
public:
    Task(int id, const std::string& name, Priority priority, TaskType type);
    virtual ~Task() = default;

    // Enforce move-only semantics for unique queue ownership
    Task(const Task&) = delete;
    Task& operator=(const Task&) = delete;
    Task(Task&&) = default;
    Task& operator=(Task&&) = default;

    /**
     * @brief Executes task logic. Must be implemented by derived classes.
     */
    virtual void execute() = 0;

    // Getters
    int getId() const { return id_; }
    std::string getName() const { return name_; }
    Priority getPriority() const { return priority_; }
    TaskType getType() const { return type_; }
    std::shared_ptr<TaskResult> getResult() const { return result_; }

    std::string getPriorityString() const;
    std::string getTypeString() const;

protected:
    int id_;
    std::string name_;
    Priority priority_;
    TaskType type_;
    std::shared_ptr<TaskResult> result_;
};

/**
 * @brief Task simulating CPU-bound work.
 */
class ComputeTask : public Task {
public:
    ComputeTask(int id, const std::string& name, Priority priority, int complexity);
    void execute() override;

private:
    int complexity_;
};

/**
 * @brief Task simulating file I/O operations.
 */
class FileIoTask : public Task {
public:
    FileIoTask(int id, const std::string& name, Priority priority, const std::string& filePath);
    void execute() override;

private:
    std::string filePath_;
};

/**
 * @brief Task simulating network requests.
 */
class NetworkTask : public Task {
public:
    NetworkTask(int id, const std::string& name, Priority priority, const std::string& url);
    void execute() override;

private:
    std::string url_;
};

#endif // TASK_H