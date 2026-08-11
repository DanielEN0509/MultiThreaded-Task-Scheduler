#include "TaskFactory.h"
#include <atomic>
#include <stdexcept>

// Thread-safe unique ID generator
int TaskFactory::generateNextId() {
    static std::atomic<int> currentId{1};
    return currentId.fetch_add(1, std::memory_order_relaxed);
}

std::unique_ptr<Task> TaskFactory::createTask(TaskType type,
                                               const std::string& name,
                                               Priority priority,
                                               const std::string& param) {
    int id = generateNextId();

    switch (type) {
        case TaskType::COMPUTE: {
            int complexity = 3;
            if (!param.empty()) {
                try {
                    complexity = std::stoi(param);
                } catch (...) {
                    complexity = 3;
                }
            }
            return std::make_unique<ComputeTask>(id, name, priority, complexity);
        }

        case TaskType::FILE_IO: {
            std::string path = param.empty() ? "/tmp/data_log.bin" : param;
            return std::make_unique<FileIoTask>(id, name, priority, path);
        }

        case TaskType::NETWORK: {
            std::string url = param.empty() ? "https://api.system.internal/v1/sync" : param;
            return std::make_unique<NetworkTask>(id, name, priority, url);
        }

        default:
            throw std::invalid_argument("Unknown TaskType provided to TaskFactory");
    }
}