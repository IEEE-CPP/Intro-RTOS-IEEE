#include "StatusLed.hpp"
#include "pw_assert/check.h"

#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(StatusLed);

void StatusLed::configure(const gpio_dt_spec *led) {
    
    PW_CHECK(led != nullptr, "LED device pointer is null");
    PW_CHECK(gpio_is_ready_dt(led), "LED device is not ready");

    PW_CHECK(gpio_pin_configure_dt(led, GPIO_OUTPUT_INACTIVE) == 0,
             "Failed to configure LED pin");
    
    this->m_led = led;
}

void StatusLed::toggle() {
    PW_CHECK(m_led != nullptr, "LED device pointer is null");
    
    // Send a toggle command to the LED pin and check for errors
    int toggle_status = gpio_pin_toggle_dt(m_led);
    PW_CHECK(toggle_status == 0, "Failed to toggle LED pin");
}
