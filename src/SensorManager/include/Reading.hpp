#ifndef READING_HPP
#define READING_HPP

#include <cstdint>

struct Reading {
    float pressure_kpa = 0.0f;
    float temperature_c = 0.0f;
    float altitude_m = 0.0f;
    uint64_t timestamp_ms = 0;
    uint32_t sequence = 0;

    // Failure Handling
    int error = 0;
    bool calibrated = false;
};

#endif // READING_HPP