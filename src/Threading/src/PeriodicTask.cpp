#include "PeriodicTask.hpp"

#include <utility>

#include <zephyr/logging/log.h>

#include "pw_assert/check.h"
#include "pw_thread/sleep.h"
#include "pw_thread/thread.h"
#include "pw_thread_zephyr/options.h"

LOG_MODULE_REGISTER(PeriodicTask);

PeriodicTask::PeriodicTask(const char *name, int priority,
                           std::chrono::milliseconds interval, Action action)
    : m_name(name),
      m_priority(priority),
      m_interval(Clock::for_at_least(interval)),
      m_action(std::move(action)) {
    
    // Validate the task identity and work function first.
    PW_CHECK(m_name != nullptr, "Task name is required");
    PW_CHECK(static_cast<bool>(m_action), "Task action is required");

    // Validate scheduling parameters before the task can be started.
    PW_CHECK(m_interval > Clock::duration::zero(),
             "Task interval must be positive");
    
    PW_CHECK(m_priority >= pw::thread::zephyr::kHighestPriority &&
                 m_priority <= pw::thread::zephyr::kLowestPriority,
             "Task priority is outside the configured range");
}

void PeriodicTask::start() {
    PW_CHECK(!m_started, "Task can only be started once");
    m_started = true;
    
    LOG_INF(
        "TASK | name=%s | priority=%d | interval=%lld ms | stack=%u B", m_name,
        m_priority,
        static_cast<long long>(
            std::chrono::duration_cast<std::chrono::milliseconds>(m_interval)
                .count()),
        static_cast<unsigned>(StackSize));

    pw::Thread(
        pw::thread::zephyr::Options(m_context).set_name(m_name).set_priority(
            m_priority),
        [this] { run(); })
        .detach();
}

uint32_t PeriodicTask::missedRuns() const { return m_missed_runs.load(); }

void PeriodicTask::run() {
    m_next_run = Clock::now();
    while (true) {
        m_action();
        waitUntilNextRun();
    }
}

void PeriodicTask::waitUntilNextRun() {
    m_next_run += m_interval;
    const auto now = Clock::now();
    if (m_next_run < now) {
        const auto skipped = (now - m_next_run) / m_interval + 1;
        m_next_run += m_interval * skipped;
        m_missed_runs.fetch_add(static_cast<uint32_t>(skipped));
    }
    pw::this_thread::sleep_until(m_next_run);
}
