#include "SharedSensorData.hpp"

#include <mutex>

void SharedSensorData::update(const Reading &reading) {
    std::lock_guard<pw::sync::Mutex> lock(m_mutex);
    m_data.last_error = reading.error;
    if (reading.error < 0) {
        ++m_data.failed_reads;
        return; // Preserve the last successful measurement and its timestamp.
    }
    if (!reading.calibrated) {
        return;
    }

    const uint32_t sequence = m_data.last_valid_reading.sequence + 1;
    m_data.last_valid_reading = reading;
    m_data.last_valid_reading.sequence = sequence;
}

SharedSensorData::Snapshot SharedSensorData::read() const {
    std::lock_guard<pw::sync::Mutex> lock(m_mutex);
    return m_data;
}
