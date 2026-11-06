#include "SensorManager.hpp"
#include "Reading.hpp"

#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(main);

#define BMP280_NODE DT_NODELABEL(bmp280)

// Get the BMP280 device
static const struct device *bmp280_dev =
    DEVICE_DT_GET(BMP280_NODE);

int main(void) {
    static SensorManager sensorManager;

    sensorManager.configure(bmp280_dev);

    while (true) {
        const Reading reading = sensorManager.readSensorData();
        LOG_INF("pressure=%.3f kPa | temperature=%.2f C | altitude=%.2f m",
                static_cast<double>(reading.pressure_kpa),
                static_cast<double>(reading.temperature_c),
                static_cast<double>(reading.altitude_m));
        k_sleep(K_SECONDS(1));
    }
}
