# BULLET UAV Targeting System

add_library(targeting_core
    core/targeting_core.cpp
)

target_include_directories(targeting_core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${OpenCV_INCLUDE_DIRS}
)

target_link_libraries(targeting_core
    Threads::Threads
    ${OpenCV_LIBS}
)
