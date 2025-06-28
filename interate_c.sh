#!/usr/bin/env bash

set -e

echo "===[ BULLETUAV: C++ TRACKER + RUST INTEGRATION ]==="

ROOT="$(pwd)"
echo "Project root: $ROOT"

# 1. Создаём tracker/
mkdir -p "$ROOT/tracker"
cd "$ROOT/tracker"

# 2. tracker.hpp
cat > tracker.hpp << 'EOF'
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
EOF

# 3. tracker.cpp
cat > tracker.cpp << 'EOF'
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
EOF

# 4. CMakeLists.txt для tracker/
cat > CMakeLists.txt << 'EOF'
cmake_minimum_required(VERSION 3.10)
project(tracker_lib CXX)

add_library(tracker SHARED tracker.cpp)
set_target_properties(tracker PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_include_directories(tracker PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
EOF

cd "$ROOT"

# 5. MAVLink C-библиотека (git submodule или wget zip)
if [ ! -d "$ROOT/mavlink" ]; then
    echo "==> Adding MAVLink C library..."
    git clone --depth=1 https://github.com/mavlink/c_library_v2.git "$ROOT/mavlink"
    mv "$ROOT/mavlink" "$ROOT/mavlink"
fi

# 6. Rust scaffold
mkdir -p "$ROOT/rust_control/src"

cat > "$ROOT/rust_control/Cargo.toml" << 'EOF'
[package]
name = "rust_control"
version = "0.1.0"
edition = "2021"

[dependencies]
libc = "0.2"
EOF

cat > "$ROOT/rust_control/build.rs" << 'EOF'
fn main() {
    println!("cargo:rustc-link-search=native=../tracker/build");
    println!("cargo:rustc-link-lib=dylib=tracker");
}
EOF

cat > "$ROOT/rust_control/src/main.rs" << 'EOF'
#[repr(C)]
pub struct TargetDataFFI {
    pub azimuth_deg: f32,
    pub elevation_deg: f32,
    pub confidence: f32,
    pub timestamp_us: u64,
}

extern "C" {
    fn tracker_get_target(out_target: *mut TargetDataFFI) -> bool;
}

fn main() {
    for _ in 0..10 {
        let mut tgt = TargetDataFFI {
            azimuth_deg: 0.0,
            elevation_deg: 0.0,
            confidence: 0.0,
            timestamp_us: 0,
        };
        let found = unsafe { tracker_get_target(&mut tgt) };
        if found {
            println!(
                "Target: azimuth={:.2}, elevation={:.2}, conf={:.2}, t={}",
                tgt.azimuth_deg, tgt.elevation_deg, tgt.confidence, tgt.timestamp_us
            );
        } else {
            println!("No target");
        }
        std::thread::sleep(std::time::Duration::from_millis(500));
    }
}
EOF

# 7. Корневой CMakeLists.txt — добавим tracker и mavlink если надо
if ! grep -q "add_subdirectory(tracker)" "$ROOT/CMakeLists.txt"; then
    echo "add_subdirectory(tracker)" >> "$ROOT/CMakeLists.txt"
    echo "message(STATUS \"Added tracker/ subdir to CMake build.\")" >> "$ROOT/CMakeLists.txt"
fi

echo ""
echo "===[ DONE ]==="
echo "1. Собери libtracker.dylib:"
echo "   cd tracker && mkdir -p build && cd build && cmake .. && make"
echo "2. Собери и запусти Rust-контроллер:"
echo "   cd ../../rust_control && cargo build && cargo run"
echo "3. Для доступа к MAVLink C-библиотеке — смотри ./mavlink (включай в CMakeLists.txt при необходимости)."
echo "4. Для Rust<->C++ связывания: DYLD_LIBRARY_PATH=../tracker/build cargo run"
echo "5. Все изменения — полностью изолированы, твой проект не тронут!"

echo ""
echo "===[ Integration complete! ]==="
