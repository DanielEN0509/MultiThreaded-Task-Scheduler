#ifndef TASK_FACTORY_H
#define TASK_FACTORY_H

#include "Task.h"
#include <memory>
#include <string>

/**
 * @brief Factory class for instantiating Task objects using Factory Method pattern.
 */
class TaskFactory {
public:
    TaskFactory() = delete;

    /**
     * @brief Creates a new Task instance wrapped in std::unique_ptr for clear ownership.
     */
    static std::unique_ptr<Task> createTask(TaskType type,
                                           const std::string& name,
                                           Priority priority,
                                           const std::string& param = "");

private:
    static int generateNextId();
};

#endif // TASK_FACTORY_H