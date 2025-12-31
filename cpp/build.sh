#!/bin/bash
# build.sh - Build script for M18 Protocol C++ project

set -e

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
PROJECT_NAME="m18-protocol"

echo "================================================"
echo "M18 Protocol - C++ Build Script"
echo "================================================"
echo ""

# Check for required tools
if ! command -v g++ &> /dev/null; then
    echo "Error: g++ compiler not found"
    echo "Install with: sudo apt-get install build-essential"
    exit 1
fi

# Parse arguments
BUILD_TYPE="Release"
CLEAN_BUILD=false
VERBOSE=false

while [[ $# -gt 0 ]]; do
    case $1 in
        -d|--debug)
            BUILD_TYPE="Debug"
            shift
            ;;
        -c|--clean)
            CLEAN_BUILD=true
            shift
            ;;
        -v|--verbose)
            VERBOSE=true
            shift
            ;;
        -h|--help)
            echo "Usage: $0 [OPTIONS]"
            echo ""
            echo "Options:"
            echo "  -d, --debug       Build in Debug mode (default: Release)"
            echo "  -c, --clean       Clean before building"
            echo "  -v, --verbose     Show verbose build output"
            echo "  -h, --help        Show this help message"
            exit 0
            ;;
        *)
            echo "Unknown option: $1"
            exit 1
            ;;
    esac
done

# Clean if requested
if [ "$CLEAN_BUILD" = true ]; then
    echo "Cleaning previous build..."
    rm -rf "$SCRIPT_DIR/build"
fi

# Create build directory
mkdir -p "$SCRIPT_DIR/build"
cd "$SCRIPT_DIR/build"

# Run CMake
echo "Configuring with CMake (BUILD_TYPE=$BUILD_TYPE)..."
if [ "$VERBOSE" = true ]; then
    cmake -DCMAKE_BUILD_TYPE=$BUILD_TYPE -DCMAKE_VERBOSE_MAKEFILE=ON "$SCRIPT_DIR"
else
    cmake -DCMAKE_BUILD_TYPE=$BUILD_TYPE "$SCRIPT_DIR"
fi

# Build
echo "Building..."
if [ "$VERBOSE" = true ]; then
    make VERBOSE=1
else
    make
fi

echo ""
echo "================================================"
echo "Build Complete!"
echo "================================================"
echo ""
echo "Executable: $SCRIPT_DIR/build/bin/m18"
echo ""
echo "Usage examples:"
echo "  $SCRIPT_DIR/build/bin/m18 --help"
echo "  $SCRIPT_DIR/build/bin/m18 --port /dev/ttyUSB0"
echo "  $SCRIPT_DIR/build/bin/m18 --port /dev/ttyUSB0 --health"
echo ""
