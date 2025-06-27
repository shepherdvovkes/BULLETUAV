#!/bin/bash

# Run BULLET UAV unit tests

echo "Running BULLET UAV unit tests..."

cd build

# Run targeting tests
./tests/unit/targeting_tests

# Run sensor tests  
./tests/unit/sensor_tests

# Run guidance tests
./tests/unit/guidance_tests

echo "Unit tests complete."
