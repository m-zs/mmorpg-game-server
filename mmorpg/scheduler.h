#pragma once
#include <queue>
#include <functional>
#include <chrono>
#include <mutex>
#include <atomic>
#include <thread>

class Scheduler {
public:
    using Clock = std::chrono::steady_clock;
    using TimePoint = Clock::time_point;
    using Task = std::function<void()>; // Tasks are stored as lambda functions

    Scheduler();
    ~Scheduler();

    // Schedule a task after a delay (e.g., schedule(1000, []{ movePlayer(); });)
    void schedule(uint64_t delayMs, Task task);

    // Process all due tasks
    void processEvents();

    // Stop the scheduler
    void shutdown();

    void run();

private:
    struct ScheduledTask {
        TimePoint execTime;  // When the task should run
        uint64_t id;         // Unique ID for cancellation (not used here)
        Task task;           // The function to execute

        // Priority queue orders tasks by execTime (earliest first)
        bool operator<(const ScheduledTask& other) const {
            return execTime > other.execTime; // Min-heap behavior
        }
    };

    std::priority_queue<ScheduledTask> m_tasks; // Stores pending tasks
    std::mutex m_mutex;                         // Thread safety
    std::atomic<uint64_t> m_nextId{ 1 };          // Auto-incrementing task ID
    std::atomic<bool> m_running{ false };         // Controls the scheduler thread
    std::thread m_thread;                       // Background thread for processing
};
