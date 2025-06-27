# BULLET UAV Targeting System

add_library(targeting_core
    core/targeting_core.cpp
    detection/target_detection.cpp
    tracking/multi_target_tracker.cpp
    prioritization/threat_prioritizer.cpp
    homing/homing_system.cpp
    search/autonomous_search.cpp
)

target_include_directories(targeting_core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(targeting_core
    Threads::Threads
)
