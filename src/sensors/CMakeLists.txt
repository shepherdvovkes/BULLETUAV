# BULLET UAV Sensors System

# Optical tracking module
add_library(optical_tracker
    optical/optical_tracker.cpp
)

target_include_directories(optical_tracker PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${OpenCV_INCLUDE_DIRS}
)

target_link_libraries(optical_tracker
    targeting_core
    ${OpenCV_LIBS}
)

# Enhanced optical tracking module
add_library(enhanced_optical_tracker
    optical/enhanced_optical_tracker.cpp
)

target_include_directories(enhanced_optical_tracker PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${OpenCV_INCLUDE_DIRS}
)

target_link_libraries(enhanced_optical_tracker
    targeting_core
    ${OpenCV_LIBS}
) 