#ifndef PERIODIC_TASK_HPP
#define PERIODIC_TASK_HPP

#include <atomic>
#include <chrono>
#include <cstddef>
#include <cstdint>

#include "pw_chrono/system_clock.h"
#include "pw_function/function.h"
#include "pw_thread_zephyr/context.h"

// Runs a supplied action at a fixed interval.
class PeriodicTask {
  public:
    using Action = pw::Function<void()>;

    PeriodicTask(const char *name, int priority,
                 std::chrono::milliseconds interval, Action action);
    void start();
    uint32_t missedRuns() const;

  private:
    using Clock = pw::chrono::SystemClock;
    static constexpr size_t StackSize = 3072;

    void run();
    void waitUntilNextRun();

    const char *m_name;
    int m_priority;
    Clock::duration m_interval;
    Action m_action;
    Clock::time_point m_next_run{};
    std::atomic<uint32_t> m_missed_runs{0};
    bool m_started = false;
    pw::thread::zephyr::ContextWithStack<StackSize> m_context;
};

#endif // PERIODIC_TASK_HPP
