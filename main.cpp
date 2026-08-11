#include "TaskFactory.h"
#include "ThreadPool.h"
#include "Scheduler.h"
#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <limits>
#include <string>

// ANSI Escape Codes for stylized Terminal Output
namespace Color {
    const char* RESET   = "\033[0m";
    const char* BOLD    = "\033[1m";
    const char* RED     = "\033[31m";
    const char* GREEN   = "\033[32m";
    const char* YELLOW  = "\033[33m";
    const char* BLUE    = "\033[34m";
    const char* CYAN    = "\033[36m";
}

/**
 * @brief Displays engine header banner.
 */
void printHeader() {
    std::cout << Color::CYAN << Color::BOLD;
    std::cout << "\n=====================================================================\n";
    std::cout << "        C++ MULTI-THREADED TASK SCHEDULER & THREAD POOL ENGINE       \n";
    std::cout << "=====================================================================\n";
    std::cout << Color::RESET;
}

/**
 * @brief Displays real-time telemetry and dashboard statistics.
 */
void printDashboard(const ThreadPool& pool, const Scheduler& scheduler) {
    std::cout << "\n" << Color::YELLOW << Color::BOLD << "┌── [SYSTEM DASHBOARD] ──────────────────────────────────────────────┐" << Color::RESET << "\n";
    std::cout << "  • Thread Pool Size      : " << pool.getThreadCount() << " worker threads\n";
    std::cout << "  • Active Worker Threads : " << pool.getActiveWorkerCount() << "\n";
    std::cout << "  • Pending Tasks in Heap : " << scheduler.getPendingTaskCount() << "\n";
    std::cout << "  • Completed Tasks Total : " << scheduler.getCompletedTaskCount() << "\n";
    std::cout << Color::YELLOW << "└────────────────────────────────────────────────────────────────────┘" << Color::RESET << "\n";
}

/**
 * @brief Clears standard input buffer stream.
 */
void clearInputBuffer() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

/**
 * @brief Handles interactive creation and queuing of custom tasks.
 */
void handleAddTask(Scheduler& scheduler, std::vector<std::shared_ptr<TaskResult>>& resultHandles) {
    std::cout << "\n" << Color::BOLD << "--- [ADD NEW TASK] ---" << Color::RESET << "\n";

    // 1. Select Task Type
    std::cout << "Select Task Type:\n";
    std::cout << "  1. COMPUTE (CPU Intensive)\n";
    std::cout << "  2. FILE_IO (Disk Read/Write)\n";
    std::cout << "  3. NETWORK (HTTP Requests)\n";
    std::cout << "Choice (1-3): ";

    int typeChoice = 0;
    std::cin >> typeChoice;
    while (typeChoice < 1 || typeChoice > 3) {
        std::cout << Color::RED << "Invalid choice. Enter 1-3: " << Color::RESET;
        clearInputBuffer();
        std::cin >> typeChoice;
    }

    TaskType type;
    if (typeChoice == 1) type = TaskType::COMPUTE;
    else if (typeChoice == 2) type = TaskType::FILE_IO;
    else type = TaskType::NETWORK;

    // 2. Select Priority
    std::cout << "\nSelect Execution Priority:\n";
    std::cout << "  0. LOW\n";
    std::cout << "  1. MEDIUM\n";
    std::cout << "  2. HIGH\n";
    std::cout << "  3. CRITICAL\n";
    std::cout << "Choice (0-3): ";

    int priorityChoice = 0;
    std::cin >> priorityChoice;
    while (priorityChoice < 0 || priorityChoice > 3) {
        std::cout << Color::RED << "Invalid choice. Enter 0-3: " << Color::RESET;
        clearInputBuffer();
        std::cin >> priorityChoice;
    }
    Priority priority = static_cast<Priority>(priorityChoice);

    clearInputBuffer();

    // 3. Task Name
    std::cout << "\nEnter Task Name/Description: ";
    std::string name;
    std::getline(std::cin, name);
    if (name.empty()) name = "User Task";

    // 4. Task Specific Parameter
    std::string paramPrompt;
    if (type == TaskType::COMPUTE) paramPrompt = "Enter complexity level (1-10) [default 3]: ";
    else if (type == TaskType::FILE_IO) paramPrompt = "Enter file path [default /tmp/data.bin]: ";
    else paramPrompt = "Enter Target URL [default https://api.internal/v1]: ";

    std::cout << paramPrompt;
    std::string param;
    std::getline(std::cin, param);

    // Create & Submit Task using TaskFactory
    auto handle = scheduler.submitTask(TaskFactory::createTask(type, name, priority, param));
    if (handle) {
        resultHandles.push_back(handle);
        std::cout << Color::GREEN << "\n[SUCCESS] " << Color::RESET
                  << "Task #" << handle->taskId << " [" << name << "] queued in Priority Heap!\n";
    }
}

/**
 * @brief Displays completion output and execution metrics for all created tasks.
 */
void handleInspectResults(const std::vector<std::shared_ptr<TaskResult>>& resultHandles) {
    std::cout << "\n" << Color::BOLD << "--- [TASK RESULTS VERIFICATION] ---" << Color::RESET << "\n";
    if (resultHandles.empty()) {
        std::cout << Color::YELLOW << "No tasks have been created yet." << Color::RESET << "\n";
        return;
    }

    for (const auto& res : resultHandles) {
        if (res) {
            if (res->success) {
                std::cout << Color::GREEN << "[COMPLETED] " << Color::RESET
                          << "Task #" << res->taskId
                          << " | Time: " << res->executionTimeMs << "ms"
                          << " | Output: " << res->outputMessage << "\n";
            } else {
                std::cout << Color::YELLOW << "[PENDING/RUNNING] " << Color::RESET
                          << "Task #" << res->taskId << " has not finished execution yet.\n";
            }
        }
    }
}

/**
 * @brief Main entry point running interactive CLI menu loop.
 */
int main() {
    printHeader();

    size_t numThreads = std::thread::hardware_concurrency();
    if (numThreads == 0) numThreads = 4;

    std::cout << Color::GREEN << "[SYSTEM INIT] " << Color::RESET
              << "Engine initialized with " << numThreads << " worker threads.\n";

    auto pool = std::make_shared<ThreadPool>(numThreads);
    Scheduler scheduler(pool);

    std::vector<std::shared_ptr<TaskResult>> resultHandles;

    bool running = true;
    while (running) {
        printDashboard(*pool, scheduler);

        std::cout << Color::CYAN << Color::BOLD << "MENU OPTIONS:" << Color::RESET << "\n";
        std::cout << "  1. Add a custom Task (Interactive)\n";
        std::cout << "  2. Load Preset Demo Tasks\n";
        std::cout << "  3. Dispatch Next Highest-Priority Task\n";
        std::cout << "  4. Dispatch ALL Pending Tasks\n";
        std::cout << "  5. Inspect Task Results\n";
        std::cout << "  6. Exit System\n";
        std::cout << Color::BOLD << "Enter choice (1-6): " << Color::RESET;

        int menuChoice = 0;
        if (!(std::cin >> menuChoice)) {
            clearInputBuffer();
            continue;
        }

        switch (menuChoice) {
            case 1:
                handleAddTask(scheduler, resultHandles);
                break;

            case 2:
                resultHandles.push_back(scheduler.submitTask(
                    TaskFactory::createTask(TaskType::FILE_IO, "Log Analysis", Priority::LOW, "/var/log/syslog")));
                resultHandles.push_back(scheduler.submitTask(
                    TaskFactory::createTask(TaskType::COMPUTE, "Crypto Hash", Priority::HIGH, "4")));
                resultHandles.push_back(scheduler.submitTask(
                    TaskFactory::createTask(TaskType::COMPUTE, "Emergency System Check", Priority::CRITICAL, "2")));
                std::cout << Color::GREEN << "\n[SUCCESS] Loaded 3 demo tasks into Priority Heap!\n" << Color::RESET;
                break;

            case 3:
                if (scheduler.getPendingTaskCount() == 0) {
                    std::cout << Color::YELLOW << "\n[INFO] Priority heap is empty. Add tasks first.\n" << Color::RESET;
                } else {
                    scheduler.dispatchNext();
                    std::cout << Color::BLUE << "\n[SCHEDULER] Dispatched top-priority task to Worker Pool.\n" << Color::RESET;
                }
                break;

            case 4: {
                size_t count = scheduler.getPendingTaskCount();
                if (count == 0) {
                    std::cout << Color::YELLOW << "\n[INFO] No pending tasks to dispatch.\n" << Color::RESET;
                } else {
                    scheduler.dispatchAll();
                    std::cout << Color::BLUE << "\n[SCHEDULER] Dispatched all " << count << " tasks by priority!\n" << Color::RESET;

                    while (pool->getActiveWorkerCount() > 0 || scheduler.getPendingTaskCount() > 0) {
                        std::this_thread::sleep_for(std::chrono::milliseconds(50));
                    }
                    std::cout << Color::GREEN << "[COMPLETED] Execution finished across threads.\n" << Color::RESET;
                }
                break;
            }

            case 5:
                handleInspectResults(resultHandles);
                break;

            case 6:
                running = false;
                break;

            default:
                std::cout << Color::RED << "Invalid option. Please enter 1-6.\n" << Color::RESET;
                break;
        }
    }

    std::cout << "\n" << Color::CYAN << Color::BOLD;
    std::cout << "=====================================================================\n";
    std::cout << "             SHUTTING DOWN ENGINE SAFELY (RAII JOIN)                 \n";
    std::cout << "=====================================================================\n";
    std::cout << Color::RESET;

    return 0;
}