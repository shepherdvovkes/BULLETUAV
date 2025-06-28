#include "tracker.hpp"
#include <chrono>
#include <thread>
#include <random>

bool tracker_get_target(TargetDataFFI* out_target) {
    static std::default_random_engine eng;
    static std::uniform_real_distribution<float> dist(-30, 30);
    static std::uniform_real_distribution<float> conf(0.8, 1.0);

    out_target->azimuth_deg = dist(eng);
    out_target->elevation_deg = dist(eng);
    out_target->confidence = conf(eng);
    out_target->timestamp_us = std::chrono::duration_cast<std::chrono::microseconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count();

    std::this_thread::sleep_for(std::chrono::milliseconds(10)); // emulate tracker delay
    return true; // always "found"
}
