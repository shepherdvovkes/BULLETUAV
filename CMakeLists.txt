cmake_minimum_required(VERSION 3.16)
project(BULLET_UAV VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Build options
option(ENABLE_TARGETING "Build targeting system" ON)
option(ENABLE_SENSORS "Build sensor modules" ON)
option(ENABLE_GUIDANCE "Build guidance system" ON)
option(ENABLE_ENGINE "Build engine control" ON)
option(ENABLE_TESTS "Build unit tests" OFF)

# Find packages
find_package(Threads REQUIRED)

# Include directories
include_directories(src)

# Targeting system
if(ENABLE_TARGETING)
    add_subdirectory(src/targeting)
endif()

# Sensors
if(ENABLE_SENSORS)
    add_subdirectory(src/sensors)
endif()

# Guidance
if(ENABLE_GUIDANCE)
    add_subdirectory(src/guidance)
endif()

# Engine
if(ENABLE_ENGINE)
    add_subdirectory(src/engine)
endif()

# Tests
if(ENABLE_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# Documentation
find_package(Doxygen)
if(DOXYGEN_FOUND)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
    add_custom_target(doc
        ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating API documentation with Doxygen" VERBATIM
    )
endif()
