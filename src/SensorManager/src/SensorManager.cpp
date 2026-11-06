#include "pw_assert/check.h"
#include <cmath>
#include <zephyr/drivers/sensor.h>
#include <zephyr/logging/log.h>

#include "Reading.hpp"
#include "SensorManager.hpp"

// Core Constants
namespace {
    static constexpr float MinPressure = 30.0f;      // Minimum pressure in kPa
    static constexpr float MaxPressure = 110.0f;     // Maximum pressure in kPa
    static constexpr float AltitudeExponent = 0.19029495f; // 1 / 5.255
    static constexpr float AtmosphericScaleHeight = 44330.0f; // Scale height for altitude calculation 
    static constexpr uint32_t ReferenceSamples = 10; // Ref Sample #
} // namespace

LOG_MODULE_REGISTER(SensorManager);

void SensorManager::configure(const struct device *bmp280_dev) {
    // Basic Assertions to ensure valid devices
    PW_CHECK(bmp280_dev != nullptr, "BMP280 device pointer is null.");
    PW_CHECK(device_is_ready(bmp280_dev), "BMP280 device is not ready.");

    this->m_bmp280_dev = bmp280_dev;

    // UART Console Logging In Zephyr
    LOG_INF("BMP280 device configured successfully.");
}

Reading SensorManager::readSensorData() {
    PW_CHECK(m_bmp280_dev != nullptr, "BMP280 device pointer is null.");

    Reading sample;
    sensor_value pressure_val{}, temp_val{};

    // Grab Zephyr Sensor Data
    PW_CHECK(sensor_sample_fetch(m_bmp280_dev) == 0,
             "Failed to fetch BMP280 sample.");

    PW_CHECK(
        sensor_channel_get(m_bmp280_dev, SENSOR_CHAN_PRESS, &pressure_val) == 0,
        "Failed to read BMP280 pressure.");

    PW_CHECK(sensor_channel_get(m_bmp280_dev, SENSOR_CHAN_AMBIENT_TEMP,
                                &temp_val) == 0,
             "Failed to read BMP280 temperature.");

    // Convert sensor values to the appropriate units
    sample.pressure_kpa = sensor_value_to_float(&pressure_val);
    sample.temperature_c = sensor_value_to_float(&temp_val);
    sample.timestamp_ms = static_cast<uint64_t>(k_uptime_get());

    PW_CHECK(sample.pressure_kpa >= MinPressure &&
                 sample.pressure_kpa <= MaxPressure,
             "BMP280 pressure is outside the valid range.");

    // Altitude Determination logic
    if (this->m_ref_samples < ReferenceSamples) {

        this->m_pressure_sum_kpa += sample.pressure_kpa;
        ++this->m_ref_samples;

        if (this->m_ref_samples == ReferenceSamples) {
            this->m_ref_pressure_kpa =
                this->m_pressure_sum_kpa / static_cast<float>(ReferenceSamples);
        }

        // Determine Altitude After Calibration
        if (this->m_ref_samples < ReferenceSamples) {
            return sample;
        }

        LOG_INF("CALIBRATION | complete | reference=%.3f kPa | samples=%u",
                static_cast<double>(this->m_ref_pressure_kpa),
                static_cast<unsigned>(this->m_ref_samples));
    }

    PW_CHECK(this->m_ref_pressure_kpa > 0.0f, "Reference pressure is invalid.");

    const float pressure_ratio = sample.pressure_kpa / this->m_ref_pressure_kpa;
    sample.altitude_m =
        AtmosphericScaleHeight * (1.0f - std::pow(pressure_ratio, AltitudeExponent));

    if (!std::isfinite(sample.altitude_m)) {
        sample.error = -ERANGE;
        return sample;
    }

    sample.calibrated = true;
    return sample;
}
