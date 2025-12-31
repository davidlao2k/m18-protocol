#ifndef SERIAL_PORT_HPP
#define SERIAL_PORT_HPP

#include <cstdint>
#include <string>
#include <vector>
#include <memory>

class SerialPort {
public:
    SerialPort(const std::string& port, int baudrate = 4800, double timeout_seconds = 0.8);
    ~SerialPort();

    bool open();
    void close();
    bool is_open() const;

    // Write and read operations
    bool write(const std::vector<uint8_t>& data);
    std::vector<uint8_t> read(size_t num_bytes);
    
    // Control signals
    void set_dtr(bool state);
    void set_rts(bool state);
    void set_break(bool state);

    // Buffer management
    void reset_input_buffer();
    void reset_output_buffer();
    
    // Configuration
    int baudrate() const;
    std::string port_name() const;

private:
    std::string port_name_;
    int baudrate_;
    double timeout_seconds_;
    int fd_;  // File descriptor for the serial port
    
    bool configure_port();
};

#endif // SERIAL_PORT_HPP
