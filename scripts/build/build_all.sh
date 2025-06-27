#!/bin/bash

# BULLET UAV Complete Build Script

set -e  # Exit on any error

echo "========================================"
echo "BULLET UAV Complete Build System"
echo "========================================"

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Build configuration
BUILD_TYPE="Release"
BUILD_DIR="build"
PARALLEL_JOBS=$(nproc)

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        --debug)
            BUILD_TYPE="Debug"
            shift
            ;;
        --build-dir)
            BUILD_DIR="$2"
            shift 2
            ;;
        --jobs)
            PARALLEL_JOBS="$2"
            shift 2
            ;;
        *)
            echo "Unknown option: $1"
            exit 1
            ;;
    esac
done

echo -e "${BLUE}Build configuration:${NC}"
echo "  Build type: $BUILD_TYPE"
echo "  Build directory: $BUILD_DIR"
echo "  Parallel jobs: $PARALLEL_JOBS"
echo ""

# Create build directory
echo -e "${BLUE}Creating build directory...${NC}"
mkdir -p $BUILD_DIR
cd $BUILD_DIR

# Configure with CMake
echo -e "${BLUE}Configuring with CMake...${NC}"
cmake -DCMAKE_BUILD_TYPE=$BUILD_TYPE \
      -DENABLE_TARGETING=ON \
      -DENABLE_SENSORS=ON \
      -DENABLE_GUIDANCE=ON \
      -DENABLE_ENGINE=ON \
      -DENABLE_TESTS=ON \
      ..

if [ $? -eq 0 ]; then
    echo -e "${GREEN}✓ CMake configuration successful${NC}"
else
    echo -e "${RED}✗ CMake configuration failed${NC}"
    exit 1
fi

# Build all components
echo -e "${BLUE}Building all components...${NC}"

# Build targeting system
echo -e "${YELLOW}Building targeting system...${NC}"
make -j$PARALLEL_JOBS targeting_core
if [ $? -eq 0 ]; then
    echo -e "${GREEN}✓ Targeting system built successfully${NC}"
else
    echo -e "${RED}✗ Targeting system build failed${NC}"
    exit 1
fi

# Build sensor systems
echo -e "${YELLOW}Building sensor systems...${NC}"
make -j$PARALLEL_JOBS sensors
if [ $? -eq 0 ]; then
    echo -e "${GREEN}✓ Sensor systems built successfully${NC}"
else
    echo -e "${RED}✗ Sensor systems build failed${NC}"
    exit 1
fi

# Build guidance system
echo -e "${YELLOW}Building guidance system...${NC}"
make -j$PARALLEL_JOBS guidance
if [ $? -eq 0 ]; then
    echo -e "${GREEN}✓ Guidance system built successfully${NC}"
else
    echo -e "${RED}✗ Guidance system build failed${NC}"
    exit 1
fi

# Build engine control
echo -e "${YELLOW}Building engine control...${NC}"
make -j$PARALLEL_JOBS engine_control
if [ $? -eq 0 ]; then
    echo -e "${GREEN}✓ Engine control built successfully${NC}"
else
    echo -e "${RED}✗ Engine control build failed${NC}"
    exit 1
fi

# Build tests
echo -e "${YELLOW}Building test suite...${NC}"
make -j$PARALLEL_JOBS tests
if [ $? -eq 0 ]; then
    echo -e "${GREEN}✓ Test suite built successfully${NC}"
else
    echo -e "${RED}✗ Test suite build failed${NC}"
    exit 1
fi

echo ""
echo -e "${GREEN}========================================${NC}"
echo -e "${GREEN}BULLET UAV Build Complete!${NC}"
echo -e "${GREEN}========================================${NC}"
echo ""
echo "Next steps:"
echo "  1. Run tests: cd $BUILD_DIR && make test"
echo "  2. Install: cd $BUILD_DIR && make install"
echo "  3. Package: cd $BUILD_DIR && make package"
echo ""
