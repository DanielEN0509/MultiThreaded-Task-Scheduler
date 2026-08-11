#include "Task.h"
#include <thread>
#include <sstream>

Task::Task(int id, const std::string& name, Priority priority, TaskType type)
    : id_(id), name_(name), priority_(priority), type_(type),
      result_(std::make_shared<TaskResult>(id)) {}

std::string Task::getPriorityString() const {
    switch (priority_) {
        case Priority::LOW:      return "LOW";
        case Priority::MEDIUM:   return "MEDIUM";
        case Priority::HIGH:     return "HIGH";
        case Priority::CRITICAL: return "CRITICAL";
        default:                 return "UNKNOWN";
    }
}

std::string Task::getTypeString() const {
    switch (type_) {
        case TaskType::COMPUTE: return "COMPUTE";
        case TaskType::FILE_IO: return "FILE_IO";
        case TaskType::NETWORK: return "NETWORK";
        default:                return "UNKNOWN";
    }
}

// ----------------------------------------------------------------------------
// Concrete Implementations
// ----------------------------------------------------------------------------

ComputeTask::ComputeTask(int id, const std::string& name, Priority priority, int complexity)
    : Task(id, name, priority, TaskType::COMPUTE), complexity_(complexity) {}

void ComputeTask::execute() {
    auto startTime = std::chrono::high_resolution_clock::now();

    // Simulate CPU computation workload
    std::this_thread::sleep_for(std::chrono::milliseconds(complexity_ * 50));

    auto endTime = std::chrono::high_resolution_clock::now();
    long long duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();

    result_->success = true;
    result_->executionTimeMs = duration;

    std::ostringstream ss;
    ss << "ComputeTask [" << name_ << "] completed matrix hashing with complexity " << complexity_;
    result_->outputMessage = ss.str();
}

FileIoTask::FileIoTask(int id, const std::string& name, Priority priority, const std::string& filePath)
    : Task(id, name, priority, TaskType::FILE_IO), filePath_(filePath) {}

void FileIoTask::execute() {
    auto startTime = std::chrono::high_resolution_clock::now();

    // Simulate disk I/O latency
    std::this_thread::sleep_for(std::chrono::milliseconds(120));

    auto endTime = std::chrono::high_resolution_clock::now();
    long long duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();

    result_->success = true;
    result_->executionTimeMs = duration;

    std::ostringstream ss;
    ss << "FileIoTask [" << name_ << "] successfully processed file at: " << filePath_;
    result_->outputMessage = ss.str();
}

NetworkTask::NetworkTask(int id, const std::string& name, Priority priority, const std::string& url)
    : Task(id, name, priority, TaskType::NETWORK), url_(url) {}

void NetworkTask::execute() {
    auto startTime = std::chrono::high_resolution_clock::now();

    // Simulate network latency
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    auto endTime = std::chrono::high_resolution_clock::now();
    long long duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();

    result_->success = true;
    result_->executionTimeMs = duration;

    std::ostringstream ss;
    ss << "NetworkTask [" << name_ << "] HTTP 200 OK response from: " << url_;
    result_->outputMessage = ss.str();
}