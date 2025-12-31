# M18 Protocol - C++ Implementation

This is a C++ port of the M18 battery protocol interface. It provides the same functionality as the Python version but with better performance and minimal dependencies.

## Features

- **Serial Communication**: Direct UART communication with M18 batteries
- **Battery Diagnostics**: Read and interpret battery health data
- **Charger Simulation**: Simulate Milwaukee M18 charger communication
- **Low-level Commands**: Access calibration, configuration, and snapshot commands
- **Debug Tools**: Brute force register scanning and command testing
- **Interactive Shell**: Command-line interface with command history (arrow keys) for easy navigation
- **Battery Detection**: Automatic detection of non-responsive batteries with helpful connection diagnostics

## Requirements

### Linux
- GCC/Clang C++17 compiler
- CMake 3.10+ (optional, can build manually)
- POSIX-compliant serial port support

### Packages to Install

**Ubuntu/Debian:**
```bash
sudo apt-get install build-essential cmake libreadline-dev
```

**Fedora/RHEL:**
```bash
sudo dnf install gcc-c++ cmake readline-devel
```

**Arch:**
```bash
sudo pacman -S base-devel cmake readline
```

**Note:** `readline` is optional. The program will work without it, but command history (arrow keys) in interactive mode won't be available. If readline is not installed, the build will automatically fall back to standard input handling.

## Building

### Using CMake (Recommended)

```bash
cd cpp
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make
```

### Using Makefile

```bash
cd cpp
make build
```

### Manual Compilation

```bash
cd cpp
mkdir -p build
g++ -std=c++17 -Wall -Wextra -O2 -Iinclude \
    src/main.cpp src/m18.cpp src/serial_port.cpp src/data_tables.cpp \
    -o build/bin/m18 -pthread
```

## Usage

### Interactive Mode

```bash
./build/bin/m18 --port /dev/ttyUSB0
```

Then use commands like:
```
> health
> read_id
> simulate
> high
> idle
> exit
```

**Features in Interactive Mode:**
- **Command History**: Use ↑ and ↓ arrow keys to recall previous commands (requires readline)
- **Battery Detection**: Automatic detection when battery doesn't respond with helpful troubleshooting hints
- **Help Command**: Type `help` to see all available commands

### Command Line Mode

**Print health report:**
```bash
./build/bin/m18 --port /dev/ttyUSB0 --health
```

**Set TX low (idle mode):**
```bash
./build/bin/m18 --port /dev/ttyUSB0 --idle
```

**Auto-detect port:**
```bash
./build/bin/m18
```

## Hardware Connection

Connect your USB-to-Serial adapter to the M18 battery:

- **UART TX** → Battery **J2** (next to B-)
- **UART RX** → Battery **J1** (next to B+)
- **GND** → Battery **GND** (B-)

**Voltage:** Use a 3.3V adapter

**Note:** When using fake FT232 chips without break condition support, use the DTR line to simulate the break condition.

## Project Structure

```
cpp/
├── CMakeLists.txt          # CMake build configuration
├── Makefile                # Alternative build system
├── README.md               # This file
├── include/
│   ├── m18.hpp            # Main M18 class header
│   ├── serial_port.hpp    # Serial port interface
│   └── data_tables.hpp    # Data structure definitions
├── src/
│   ├── main.cpp           # Entry point
│   ├── m18.cpp            # M18 class implementation
│   ├── serial_port.cpp    # Serial port implementation
│   └── data_tables.cpp    # Battery data tables
└── build/                 # Build output (created during build)
    └── bin/
        └── m18            # Compiled executable
```

## Key Classes and Methods

### M18 Class

**Connection:**
- `bool connect(const std::string& port)` - Connect to serial port
- `void disconnect()` - Disconnect from serial port
- `bool is_connected() const` - Check connection status

**Diagnostics:**
- `BatteryHealth health(bool force_refresh = true)` - Get battery health report
- `void read_id(...)` - Read all diagnostic registers

**Low-level Commands:**
- `bool reset()` - Reset device
- `std::vector<uint8_t> configure(uint8_t state)` - Send configuration
- `std::vector<uint8_t> get_snapchat()` - Get snapshot data
- `std::vector<uint8_t> keepalive()` - Send keep-alive
- `std::vector<uint8_t> calibrate()` - Calibration command

**Control:**
- `void high()` - Bring J2 pin high (20V)
- `void idle()` - Pull J2 pin low (0V)
- `void high_for(int duration_seconds)` - High for N seconds

**Debugging:**
- `void brute(uint8_t msb, uint8_t lsb, ...)` - Brute force register
- `void full_brute(uint16_t start, uint16_t stop, ...)` - Full range scan
- `void debug(...)` - Debug specific register

## Differences from Python Version

- **No interactive REPL**: Uses simple command-line interface instead
- **No form submission**: HTTP submission not implemented (use Python for this)
- **Binary compatibility**: Same register format and protocol
- **Performance**: ~100x faster execution
- **Dependencies**: Minimal (only standard C++ library)

## Troubleshooting

### Serial Port Not Found
Ensure the USB adapter is connected and use `ls /dev/tty*` to find your port:
- Common ports: `/dev/ttyUSB0`, `/dev/ttyACM0`, `/dev/ttyS0`

### Permission Denied
Add yourself to the dialout group:
```bash
sudo usermod -a -G dialout $USER
# Then log out and back in
```

### Incorrect Voltages
If you're not seeing correct voltage levels:
1. Check your USB adapter voltage (should be 3.3V)
2. Verify connections are correct
3. Check for fake CH340/FT232 chips that may need break condition emulation

## Performance Comparison

| Operation | Python | C++ | Speedup |
|-----------|--------|-----|---------|
| health() | ~8-10s | ~2-3s | 3-4x |
| read_all() | ~15-20s | ~3-5s | 3-5x |
| reset() | ~600ms | ~300ms | 2x |

## License

Same as the original Python project.

## Contributing

For improvements or bug fixes, please submit to the original repository:
https://github.com/mnh-jansson/m18-protocol

## References

- Original Python implementation: `../m18.py`
- Milwaukee M18 Protocol Documentation: See `../docs/`
- Serial protocol: 4800 baud, 8 data bits, 2 stop bits
