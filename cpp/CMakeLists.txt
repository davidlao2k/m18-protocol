cmake_minimum_required(VERSION 3.10)
project(m18-protocol)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Source files
set(SOURCES
    src/main.cpp
    src/m18.cpp
    src/serial_port.cpp
    src/data_tables.cpp
)

# Create executable
add_executable(m18 ${SOURCES})

# Add compile flags for additional warnings and optimizations
target_compile_options(m18 PRIVATE 
    -Wall 
    -Wextra 
    -Wpedantic
    -O2
)

# Link threading library (for std::thread)
find_package(Threads REQUIRED)
target_link_libraries(m18 PRIVATE Threads::Threads)

# Optional: Add installation target
install(TARGETS m18 DESTINATION bin)

