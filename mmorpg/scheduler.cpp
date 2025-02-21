#include "scheduler.h"

Scheduler::Scheduler() {
}

Scheduler::~Scheduler() {
    shutdown();
    if (m_thread.joinable()) {
        m_thread.join();
    }
}

void Scheduler::schedule(uint64_t delayMs, Task task) {
    std::lock_guard<std::mutex> lock(m_mutex);
    TimePoint execTime = Clock::now() + std::chrono::milliseconds(delayMs);
    m_tasks.push({ execTime, m_nextId++, task });
}

void Scheduler::processEvents() {
    std::lock_guard<std::mutex> lock(m_mutex);
    TimePoint now = Clock::now();

    while (!m_tasks.empty() && m_tasks.top().execTime <= now) {
        const auto& task = m_tasks.top();
        task.task();
        m_tasks.pop();
    }
}

void Scheduler::shutdown() {
    m_running = false;
    std::lock_guard<std::mutex> lock(m_mutex);
    while (!m_tasks.empty()) {
        m_tasks.pop();
    }
}

void Scheduler::run()
{
    if (m_running.exchange(true)) return; // Prevent multiple threads

    m_thread = std::thread([this]() {
        while (m_running) {
            processEvents();
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    });
}
