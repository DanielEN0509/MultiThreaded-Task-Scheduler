# Multi-Threaded Task Scheduler & Thread Pool Engine (C++20)

A high-performance, interactive C++20 Thread Pool and Priority-Based Task Scheduler Engine designed for concurrent execution, dynamic priority management, and real-time thread synchronization.

---

## 🌟 Key Features & Technical Highlights

* **Modern C++20 & RAII:** Engineered using C++20 standards, move semantics (`std::move`), and strict RAII principles to ensure zero dynamic allocation leaks and graceful shutdowns.
* **Smart Pointer Ownership Pipeline:** 
  * Enforces exclusive task ownership in queues using `std::unique_ptr<Task>`.
  * Tracks cross-thread execution states using `std::shared_ptr<TaskResult>`.
  * Prevents circular dependency memory leaks in registry lookups via `std::weak_ptr<TaskResult>`.
* **Thread Safety & Concurrency:** Multi-threaded worker execution using `std::thread`, `std::mutex`, `std::condition_variable` (zero CPU overhead while sleeping), and `std::atomic` counters for real-time telemetry.
* **Priority Heap Scheduling:** Custom Max-Heap using `std::priority_queue` with a custom functor comparator (`TaskComparator`) to schedule `CRITICAL`, `HIGH`, `MEDIUM`, and `LOW` tasks.
* **Design Patterns:** Implements the **Factory Method** pattern (`TaskFactory`) to decouple task creation logic from scheduler execution.
* **Interactive Live CLI Dashboard:** Interactive command-line interface featuring ANSI color coding, live system metrics, dynamic input handling, and execution inspection.

---

## 🏗️ System Architecture

```text
                               ┌───────────────────────────┐
                               │        TaskFactory        │
                               └─────────────┬─────────────┘
                                             │ (creates std::unique_ptr<Task>)
                                             ▼
┌───────────────────┐             ┌─────────────────────────┐             ┌───────────────────┐
│  Interactive CLI  │ ──────────> │    Scheduler Engine     │ ──────────> │    Thread Pool    │
│  (User Input)     │             │  (Priority Heap Queue)  │             │  (Worker Threads) │
└───────────────────┘             └────────────┬────────────┘             └─────────┬─────────┘
                                               │                                    │
                                               ▼                                    ▼
                                  ┌─────────────────────────┐           ┌───────────────────────┐
                                  │     taskRegistry_       │           │ Executes task and     │
                                  │   (std::weak_ptr map)   │           │ updates TaskResult    │
                                  └─────────────────────────┘           └───────────────────────┘
