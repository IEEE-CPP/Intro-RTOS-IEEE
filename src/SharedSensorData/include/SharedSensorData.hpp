#ifndef SHARED_SENSOR_DATA_HPP
#define SHARED_SENSOR_DATA_HPP

#include "Reading.hpp"
#include "pw_sync/mutex.h"

// Thread-safe storage for sensor readings
class SharedSensorData {
  public:
    struct Snapshot {
        Reading last_valid_reading{};
        uint32_t failed_reads = 0;
        int last_error = 0;
    };

    void update(const Reading &reading);
    Snapshot read() const;

  private:
    mutable pw::sync::Mutex m_mutex;
    Snapshot m_data{};
};

#endif // SHARED_SENSOR_DATA_HPP
