#ifndef STATUS_LED_HPP
#define STATUS_LED_HPP

struct gpio_dt_spec;

class StatusLed {
  public:
    void configure(const gpio_dt_spec *led);
    void toggle();

  private:
    const gpio_dt_spec *m_led = nullptr;
};

#endif // STATUS_LED_HPP
