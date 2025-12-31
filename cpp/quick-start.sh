#!/bin/bash
# quick-start.sh - Quick setup and build guide for M18 C++ project

set -e

echo "╔════════════════════════════════════════════════════════════════════╗"
echo "║         M18 Protocol - C++ Project Quick Start                     ║"
echo "╚════════════════════════════════════════════════════════════════════╝"
echo ""

# Check prerequisites
echo "Checking prerequisites..."
if ! command -v g++ &> /dev/null; then
    echo "❌ g++ compiler not found"
    echo "   Install: sudo apt-get install build-essential cmake"
    exit 1
fi
echo "✅ g++ compiler found: $(g++ --version | head -n1)"

if ! command -v cmake &> /dev/null; then
    echo "⚠️  cmake not found (optional, can build with make)"
    HAS_CMAKE=false
else
    echo "✅ cmake found: $(cmake --version | head -n1)"
    HAS_CMAKE=true
fi

echo ""
echo "Project structure:"
echo "  m18.py                    - Original Python implementation"
echo "  cpp/                      - C++ implementation (this version)"
echo "  MIGRATION.md              - Migration guide from Python"
echo ""

cd "$(dirname "${BASH_SOURCE[0]}")/cpp" || exit 1

echo "╔════════════════════════════════════════════════════════════════════╗"
echo "║ BUILDING C++ PROJECT                                               ║"
echo "╚════════════════════════════════════════════════════════════════════╝"
echo ""

if [ "$HAS_CMAKE" = true ]; then
    echo "Method 1: Using CMake (Recommended)"
    echo "  $ cd cpp && make build"
    echo "  $ ./build/m18 --help"
    echo ""
else
    echo "Method 2: Using direct compilation"
    echo "  $ cd cpp && make build-manual"
    echo "  $ ./build/m18 --help"
    echo ""
fi

# Attempt build
echo "Building now..."
make build > /dev/null 2>&1 || {
    echo "❌ Build failed. Trying alternative method..."
    make build-manual > /dev/null 2>&1 || {
        echo "Build failed. See errors above."
        exit 1
    }
}

echo "✅ Build successful!"
echo ""

# Check executable
if [ -f build/m18 ]; then
    SIZE=$(du -h build/m18 | cut -f1)
    echo "Binary info:"
    echo "  Location: $(pwd)/build/m18"
    echo "  Size: $SIZE"
    echo ""
fi

echo "╔════════════════════════════════════════════════════════════════════╗"
echo "║ NEXT STEPS                                                         ║"
echo "╚════════════════════════════════════════════════════════════════════╝"
echo ""
echo "1. View help:"
echo "   $ ./build/m18 --help"
echo ""
echo "2. Connect your M18 battery via USB serial adapter and run:"
echo "   $ ./build/m18 --port /dev/ttyUSB0"
echo "   $ ./build/m18 --port /dev/ttyUSB0 --health"
echo ""
echo "3. Or auto-detect port:"
echo "   $ ./build/m18"
echo ""
echo "4. For development/debugging:"
echo "   $ make debug              # Build with debug symbols"
echo "   $ make run                # Build and run interactive mode"
echo ""

echo "Documentation:"
echo "  - README.md               - General M18 protocol info"
echo "  - cpp/README.md           - C++ specific documentation"
echo "  - cpp/CONVERSION_SUMMARY.md - What was converted"
echo "  - MIGRATION.md            - Python to C++ migration guide"
echo ""

echo "╔════════════════════════════════════════════════════════════════════╗"
echo "║ CONNECTION DIAGRAM                                                 ║"
echo "╚════════════════════════════════════════════════════════════════════╝"
echo ""
echo "  USB Serial Adapter          M18 Battery"
echo "  ════════════════════        ═════════════"
echo "  GND ───────────────────────► GND (B-)"
echo "  TX  ───────────────────────► J2"
echo "  RX  ───────────────────────► J1"
echo "  "
echo "  Voltage: Use 3.3V adapter"
echo "  Baud: 4800, 8 bits, 2 stop bits"
echo ""

echo "✅ Setup complete! You're ready to use M18 Protocol in C++"
echo ""
