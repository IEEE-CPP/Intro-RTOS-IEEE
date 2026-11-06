#include <chrono>
#include <cinttypes>

#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>
#include <zephyr/version.h>

#include "PeriodicTask.hpp"
#include "Reading.hpp"
#include "SensorManager.hpp"
#include "SharedSensorData.hpp"
#include "StatusLed.hpp"

LOG_MODULE_REGISTER(demo);

namespace {
constexpr uint64_t StaleAfterMs = 500;

void logSensorData(const SharedSensorData::Snapshot &snapshot) {
    const Reading &reading = snapshot.last_valid_reading;

    if (!reading.calibrated) {
        if (snapshot.last_error < 0) {
            LOG_ERR("Sensor unavailable (read error %d)", snapshot.last_error);
        } else {
            LOG_INF(
                "Sensor calibrating; keep it still until a reading is ready");
        }
        return;
    }

    const uint64_t age_ms =
        static_cast<uint64_t>(k_uptime_get()) - reading.timestamp_ms;

    LOG_INF("Sensor reading #%" PRIu32, reading.sequence);
    LOG_INF("  Pressure         : %.3f kPa",
            static_cast<double>(reading.pressure_kpa));
    LOG_INF("  Temperature      : %.2f C",
            static_cast<double>(reading.temperature_c));
    LOG_INF("  Relative altitude: %+.2f m",
            static_cast<double>(reading.altitude_m));
    LOG_INF("  Reading age      : %" PRIu64 " ms", age_ms);

    if (age_ms > StaleAfterMs) {
        LOG_WRN("  Read status      : STALE (last error %d)",
                snapshot.last_error);
    } else if (snapshot.last_error < 0) {
        LOG_WRN(
            "  Read status      : ERROR %d; displaying the last valid reading",
            snapshot.last_error);
    } else {
        LOG_INF("  Read status      : FRESH");
    }
}

void logSystemStatus(const SharedSensorData::Snapshot &snapshot,
                     uint32_t missed_runs) {
    LOG_INF("System status");
    LOG_INF("  Uptime           : %" PRIu64 " ms",
            static_cast<uint64_t>(k_uptime_get()));
    LOG_INF("  Failed reads     : %" PRIu32, snapshot.failed_reads);
    LOG_INF("  Last read error  : %d", snapshot.last_error);
    LOG_INF("  Missed task runs : %" PRIu32, missed_runs);
}
} // namespace

#define BMP280_NODE DT_NODELABEL(bmp280)

// Get the BMP280 device
static const struct device *bmp280_dev =
    DEVICE_DT_GET(BMP280_NODE);

#define LED_NODE DT_ALIAS(led0)
const gpio_dt_spec led_dev = GPIO_DT_SPEC_GET(LED_NODE, gpios);

int main() {
    using namespace std::chrono_literals;

    // Components own their hardware/resources; main only connects their APIs.
    static SensorManager sensorManager;
    static SharedSensorData sensorData;
    static StatusLed statusLed;

    sensorManager.configure(bmp280_dev);
    statusLed.configure(&led_dev);

    // The task runner accepts actions without knowing the components they use.
    static PeriodicTask samplingTask("sampling", 3, 100ms, [] {
        const Reading reading = sensorManager.readSensorData();
        sensorData.update(reading);
    });
    
    static PeriodicTask heartbeatTask("heartbeat", 5, 500ms,
                                      [] { statusLed.toggle(); });
    
    static PeriodicTask reportingTask("reporting", 7, 1s, [] {
        const auto snapshot = sensorData.read();
        logSensorData(snapshot);
        logSystemStatus(snapshot, samplingTask.missedRuns());
    });

    samplingTask.start();
    heartbeatTask.start();
    reportingTask.start();
    return 0;
}
