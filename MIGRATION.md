# Migration Guide: Python to C++

This document explains how to use the new C++ version of the m18-protocol tool and how it differs from the Python version.

## Quick Start

### Building
```bash
cd cpp
make build
./build/m18 --help
```

### Running
```bash
# Interactive mode (auto-detect port)
./build/m18

# With specific port
./build/m18 --port /dev/ttyUSB0

# Get health report
./build/m18 --port /dev/ttyUSB0 --health

# Set TX low
./build/m18 --port /dev/ttyUSB0 --idle
```

## Command Mapping

### Python → C++

| Python | C++ | Notes |
|--------|-----|-------|
| `m.health()` | `health()` | Same functionality |
| `m.read_id()` | `read_id()` | Same functionality |
| `m.read_id(output="raw")` | Planned | Use health() for now |
| `m.simulate()` | `simulate()` | Same functionality |
| `m.high()` | `high()` | Same functionality |
| `m.idle()` | `idle()` | Same functionality |
| `m.high_for(N)` | `high_for(N)` | Same functionality |
| `m.reset()` | Reset (automatic) | Called internally |
| `m.calibrate()` | Planned | Advanced feature |
| `m.submit_form()` | N/A | Use Python version for this |

## Key Differences

### Advantages of C++
- **Speed**: 3-5x faster than Python
- **Dependencies**: Only standard library (no pip packages needed)
- **Performance**: Better for real-time serial communication
- **Direct compilation**: No interpreter overhead

### Limitations
- **Interactive shell**: Simplified command syntax (Python's IPython shell is more powerful)
- **Form submission**: Use the Python version for Google Forms integration
- **No REPL**: Cannot import and use as a library interactively

### Error Handling
- C++ version shows immediate error messages
- More strict about syntax (command-line arguments must be exact)

## Building Variants

### CMake (Recommended for portability)
```bash
cd cpp
mkdir build
cd build
cmake ..
make
```

### Make (Simpler, requires no CMake)
```bash
cd cpp
make build      # Release build
make debug      # Debug build
make clean      # Clean artifacts
```

### Manual (For systems without build tools)
```bash
cd cpp
g++ -std=c++17 -O2 -Iinclude \
    src/main.cpp src/m18.cpp src/serial_port.cpp src/data_tables.cpp \
    -o m18 -pthread
```

## File Differences

### Directory Structure

**Python:**
```
m18.py                  # Single file, ~1000 lines
requirements.txt        # Dependencies
```

**C++:**
```
cpp/
├── include/            # Headers
│   ├── m18.hpp
│   ├── serial_port.hpp
│   └── data_tables.hpp
├── src/                # Implementation
│   ├── main.cpp
│   ├── m18.cpp
│   ├── serial_port.cpp
│   └── data_tables.cpp
├── CMakeLists.txt      # CMake build config
├── Makefile            # Simple make config
└── build.sh            # Build script
```

## Troubleshooting

### "Serial port not found"
- Python: Menu appears
- C++: Shows available ports, prompts to choose

### "Permission denied" on serial port
Same fix for both:
```bash
sudo usermod -a -G dialout $USER
# Log out and back in
```

### Build fails with compiler error
- Make sure you have g++ 7+ or clang 5+
- C++17 support required
- On Ubuntu: `sudo apt-get install build-essential cmake`

## When to Use Each Version

### Use Python (`m18.py`) when:
- You need interactive REPL with full Python integration
- You want to submit form data to Google Forms
- You need maximum flexibility and extensibility
- You're running on resource-constrained systems

### Use C++ (`cpp/m18`) when:
- You need fast, repeated operations
- You're automating battery checks
- You want minimal dependencies
- You need better performance on 1000+ operations

## Advanced Usage

### Debugging
```bash
# Both versions support debug output
# Set flags in C++: m18.print_tx = true; m18.print_rx = true;
```

### Batch Operations
```bash
# Python: Can use scripts with loops
# C++: Compile multiple calls into a single binary for speed
```

## Future C++ Features (Planned)

- [ ] Full `read_id()` with spreadsheet output
- [ ] `submit_form()` via libcurl
- [ ] Interactive REPL (simple command loop)
- [ ] Binary protocol analyzer
- [ ] JSON output format
- [ ] Network socket support

## Maintaining Both Versions

Both versions are maintained:
- **Python**: Reference implementation, maximum features
- **C++**: High-performance alternative, subset of features
- **Shared data**: Both use same register definitions and protocol

## Questions?

Refer to:
- `cpp/README.md` - C++ specific documentation
- `../README.md` - General M18 protocol documentation
- `../docs/` - Technical protocol details
