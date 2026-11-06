#ifndef SENSOR_MANAGER_HPP
#define SENSOR_MANAGER_HPP

#include <cstdint>

// Forward declaration of the Zephyr Device Struct.
// This allows us to prevent header collisions.
struct device;
struct Reading;

class SensorManager {
  public:
    SensorManager() = default;

    void configure(const struct device *bmp280_dev);

    Reading readSensorData();

    ~SensorManager() = default;

  private:
    const device *m_bmp280_dev = nullptr;
    uint32_t m_ref_samples = 0;
    float m_pressure_sum_kpa = 0.0f;
    float m_ref_pressure_kpa = 0.0f;
};

#endif // SENSOR_MANAGER_HPP
