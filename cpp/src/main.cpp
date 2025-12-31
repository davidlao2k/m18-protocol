#include "m18.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <cstring>

void print_help() {
    std::cout << R"(M18 Protocol Interface

Usage: m18 [OPTIONS]

OPTIONS:
  --port PORT              Serial port to connect to (e.g., /dev/ttyUSB0)
  --health                 Print health report and exit
  --idle                   Set TX=Low and exit (prevents charge increments)
  --interactive            Enter interactive shell (default)
  --help                   Show this help message

COMMANDS (in interactive shell):
  m.health()               Print simple health report on battery
  m.read_id()              Print labelled and formatted diagnostics
  m.simulate()             Simulate charging communication
  m.high()                 Bring J2 pin high (20V)
  m.idle()                 Pull J2 pin low (0V)
  m.high_for(N)            Bring J2 high for N seconds then idle
  m.help()                 Show command help
  
Connect UART-TX to M18-J2 and UART-RX to M18-J1 to fake the charger
Connect UART-GND to M18-GND
)" << std::endl;
}

int main(int argc, char* argv[]) {
    std::string port;
    bool health_mode = false;
    bool idle_mode = false;
    bool interactive = true;
    bool help_requested = false;

    // Parse command line arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--port" && i + 1 < argc) {
            port = argv[++i];
        } else if (arg == "--health") {
            health_mode = true;
            interactive = false;
        } else if (arg == "--idle") {
            idle_mode = true;
            interactive = false;
        } else if (arg == "--help") {
            help_requested = true;
            interactive = false;
        } else if (arg == "--interactive") {
            interactive = true;
        }
    }

    if (help_requested) {
        print_help();
        return 0;
    }

    try {
        // Create M18 instance
        M18 m18;

        // Connect to port
        if (port.empty()) {
            std::cout << "*** NO PORT SPECIFIED ***" << std::endl;
            port = m18.select_port();
        }

        if (!m18.connect(port)) {
            std::cerr << "Failed to connect to " << port << std::endl;
            return 1;
        }

        std::cout << "Connected to " << port << std::endl;

        // Execute appropriate mode
        if (idle_mode) {
            m18.idle();
            std::cout << "TX should now be low voltage (<1V). Safe to connect" << std::endl;
        } else if (health_mode) {
            std::cout << "Reading battery health..." << std::endl;
            auto health = m18.health();
            std::cout << "Health report completed" << std::endl;
        } else if (interactive) {
            std::cout << R"(
Entering interactive shell...
Available commands:
  health()              - Print simple health report
  read_id()             - Print all diagnostics
  simulate()            - Simulate charging
  high()                - Bring J2 high
  idle()                - Bring J2 low
  high_for(seconds)     - High for N seconds
  help()                - Show help
  exit()                - Exit
  
Try: health()
)" << std::endl;

            // Simple interactive shell
            std::string command;
            while (true) {
                std::cout << "> ";
                std::getline(std::cin, command);

                if (command == "exit" || command == "quit") {
                    break;
                } else if (command == "health()") {
                    m18.health();
                } else if (command == "read_id()") {
                    m18.read_id();
                } else if (command == "simulate()") {
                    m18.simulate();
                } else if (command == "high()") {
                    m18.high();
                    std::cout << "J2 is now high (20V)" << std::endl;
                } else if (command == "idle()") {
                    m18.idle();
                    std::cout << "J2 is now low (0V)" << std::endl;
                } else if (command.substr(0, 10) == "high_for(") {
                    try {
                        int seconds = std::stoi(command.substr(10));
                        m18.high_for(seconds);
                        std::cout << "J2 was high for " << seconds << " seconds" << std::endl;
                    } catch (...) {
                        std::cout << "Usage: high_for(seconds)" << std::endl;
                    }
                } else if (command == "help()") {
                    std::cout << R"(Available commands:
  health()              - Print simple health report on battery
  read_id()             - Print all registers in labelled format
  simulate()            - Simulate charger communication
  high()                - Bring J2 pin high (20V)
  idle()                - Pull J2 pin low (0V)
  high_for(N)           - Bring J2 high for N seconds then idle
  exit() or quit()      - Exit the program
)" << std::endl;
                } else if (!command.empty()) {
                    std::cout << "Unknown command. Type help() for commands." << std::endl;
                }
            }
        }

        m18.disconnect();
        std::cout << "Disconnected" << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
