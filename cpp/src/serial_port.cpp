#include "serial_port.hpp"
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <cstring>
#include <stdexcept>
#include <thread>
#include <chrono>

SerialPort::SerialPort(const std::string& port, int baudrate, double timeout_seconds)
    : port_name_(port), baudrate_(baudrate), timeout_seconds_(timeout_seconds), fd_(-1) {
}

SerialPort::~SerialPort() {
    if (is_open()) {
        close();
    }
}

bool SerialPort::open() {
    if (is_open()) {
        return true;
    }

    fd_ = ::open(port_name_.c_str(), O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (fd_ < 0) {
        throw std::runtime_error("Failed to open serial port: " + std::string(strerror(errno)));
    }

    if (!configure_port()) {
        ::close(fd_);
        fd_ = -1;
        return false;
    }

    return true;
}

bool SerialPort::configure_port() {
    struct termios tty;
    
    if (tcgetattr(fd_, &tty) != 0) {
        throw std::runtime_error("Failed to get terminal attributes: " + std::string(strerror(errno)));
    }

    // Configure baud rate
    speed_t baud;
    switch (baudrate_) {
        case 9600:
            baud = B9600;
            break;
        case 4800:
            baud = B4800;
            break;
        case 19200:
            baud = B19200;
            break;
        case 38400:
            baud = B38400;
            break;
        case 115200:
            baud = B115200;
            break;
        default:
            baud = B4800;
    }

    cfsetospeed(&tty, baud);
    cfsetispeed(&tty, baud);

    // 8 data bits, 1 stop bit, no parity
    tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;
    tty.c_cflag |= CSTOPB;  // 2 stop bits
    tty.c_cflag &= ~PARENB;  // no parity

    // Disable flow control
    tty.c_cflag &= ~CRTSCTS;
    tty.c_cflag |= CREAD | CLOCAL;

    // Non-canonical mode, no echo
    tty.c_lflag = 0;
    tty.c_oflag = 0;
    tty.c_iflag = 0;

    // Set timeout
    tty.c_cc[VMIN] = 0;
    tty.c_cc[VTIME] = static_cast<cc_t>(timeout_seconds_ * 10);  // timeout in deciseconds

    if (tcsetattr(fd_, TCSANOW, &tty) != 0) {
        throw std::runtime_error("Failed to set terminal attributes: " + std::string(strerror(errno)));
    }

    return true;
}

void SerialPort::close() {
    if (is_open()) {
        ::close(fd_);
        fd_ = -1;
    }
}

bool SerialPort::is_open() const {
    return fd_ >= 0;
}

bool SerialPort::write(const std::vector<uint8_t>& data) {
    if (!is_open()) {
        throw std::runtime_error("Serial port is not open");
    }

    ssize_t bytes_written = ::write(fd_, data.data(), data.size());
    if (bytes_written < 0) {
        throw std::runtime_error("Failed to write to serial port: " + std::string(strerror(errno)));
    }

    return static_cast<size_t>(bytes_written) == data.size();
}

std::vector<uint8_t> SerialPort::read(size_t num_bytes) {
    if (!is_open()) {
        throw std::runtime_error("Serial port is not open");
    }

    std::vector<uint8_t> buffer(num_bytes);
    ssize_t bytes_read = ::read(fd_, buffer.data(), num_bytes);

    if (bytes_read < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            return std::vector<uint8_t>();  // timeout, return empty
        }
        throw std::runtime_error("Failed to read from serial port: " + std::string(strerror(errno)));
    }

    buffer.resize(bytes_read);
    return buffer;
}

void SerialPort::set_dtr(bool state) {
    if (!is_open()) {
        throw std::runtime_error("Serial port is not open");
    }

    int lines = TIOCM_DTR;
    if (state) {
        ioctl(fd_, TIOCMBIS, &lines);  // Set DTR
    } else {
        ioctl(fd_, TIOCMBIC, &lines);  // Clear DTR
    }
}

void SerialPort::set_rts(bool state) {
    if (!is_open()) {
        throw std::runtime_error("Serial port is not open");
    }

    int lines = TIOCM_RTS;
    if (state) {
        ioctl(fd_, TIOCMBIS, &lines);  // Set RTS
    } else {
        ioctl(fd_, TIOCMBIC, &lines);  // Clear RTS
    }
}

void SerialPort::set_break(bool state) {
    if (!is_open()) {
        throw std::runtime_error("Serial port is not open");
    }

    if (state) {
        ioctl(fd_, TIOCSBRK);  // Set break
    } else {
        ioctl(fd_, TIOCCBRK);  // Clear break
    }
}

void SerialPort::reset_input_buffer() {
    if (is_open()) {
        tcflush(fd_, TCIFLUSH);
    }
}

void SerialPort::reset_output_buffer() {
    if (is_open()) {
        tcflush(fd_, TCOFLUSH);
    }
}

int SerialPort::baudrate() const {
    return baudrate_;
}

std::string SerialPort::port_name() const {
    return port_name_;
}
