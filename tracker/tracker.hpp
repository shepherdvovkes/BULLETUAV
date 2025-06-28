#pragma once
#include <cstdint>

struct TargetData {
    float azimuth_deg;
    float elevation_deg;
    float confidence;
    uint64_t timestamp_us;
};

extern "C" {
    struct TargetDataFFI {
        float azimuth_deg;
        float elevation_deg;
        float confidence;
        uint64_t timestamp_us;
    };
    bool tracker_get_target(TargetDataFFI* out_target);
}
