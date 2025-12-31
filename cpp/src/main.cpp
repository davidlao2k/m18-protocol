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
  health                   Print simple health report on battery
  read_id                  Print labelled and formatted diagnostics
  simulate                 Simulate charging communication
  high                     Bring J2 pin high (20V)
  idle                     Pull J2 pin low (0V)
  high_for N               Bring J2 high for N seconds then idle
  help                     Show command help
  
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
        
        // Test if battery is responding
        try {
            if (!m18.reset()) {
                std::cout << "\nWARNING: Battery may not be responding" << std::endl;
                std::cout << "Check connections and battery power" << std::endl;
                std::cout << "Continuing anyway..." << std::endl;
            }
        } catch (const std::exception& e) {
            std::cout << "\nWARNING: Battery communication test failed: " << e.what() << std::endl;
            std::cout << "Check connections: UART-TX->J2, UART-RX->J1, GND->GND" << std::endl;
            std::cout << "Continuing anyway..." << std::endl;
        }

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
  health              - Print simple health report
  read_id             - Print all diagnostics
  simulate            - Simulate charging
  high                - Bring J2 high
  idle                - Bring J2 low
  high_for N          - High for N seconds
  help                - Show help
  exit                - Exit
  
Try: health
)" << std::endl;

            // Simple interactive shell
            std::string command;
            while (true) {
                std::cout << "> ";
                std::getline(std::cin, command);

                // Trim whitespace
                command.erase(0, command.find_first_not_of(" \t"));
                command.erase(command.find_last_not_of(" \t") + 1);

                if (command == "exit" || command == "quit") {
                    break;
                } else if (command == "health") {
                    try {
                        auto health = m18.health();
                        if (health.type.empty() && health.model.empty()) {
                            std::cout << "Warning: Battery not responding or no data available" << std::endl;
                        } else {
                            std::cout << "Battery Health Report:" << std::endl;
                            std::cout << "  Type: " << health.type << std::endl;
                            std::cout << "  Model: " << health.model << std::endl;
                            std::cout << "  Serial: " << health.serial << std::endl;
                            std::cout << "  Pack Voltage: " << health.pack_voltage << "V" << std::endl;
                            std::cout << "  Temperature: " << health.temperature << "°C" << std::endl;
                            std::cout << "  Total Discharge: " << health.total_discharge_ah << "Ah" << std::endl;
                        }
                    } catch (const std::exception& e) {
                        std::cout << "Error reading battery health: " << e.what() << std::endl;
                        std::cout << "Check connections: UART-TX to J2, UART-RX to J1, UART-GND to GND" << std::endl;
                    }
                } else if (command == "read_id") {
                    try {
                        std::cout << "Reading battery diagnostics..." << std::endl;
                        m18.read_id();
                        std::cout << "Read ID completed" << std::endl;
                    } catch (const std::exception& e) {
                        std::cout << "Error reading diagnostics: " << e.what() << std::endl;
                        std::cout << "\nDiagnostics failed. Checking battery connection..." << std::endl;
                        
                        // Try a reset to see if battery responds
                        try {
                            if (m18.reset()) {
                                std::cout << "Battery is responding to reset command" << std::endl;
                                std::cout << "Try 'read_id' again" << std::endl;
                            } else {
                                std::cout << "Battery did not respond to reset" << std::endl;
                                std::cout << "Check connections: UART-TX->J2, UART-RX->J1, GND->GND" << std::endl;
                            }
                        } catch (const std::exception& reset_e) {
                            std::cout << "Battery not responding: " << reset_e.what() << std::endl;
                            std::cout << "Check connections: UART-TX->J2, UART-RX->J1, GND->GND" << std::endl;
                            std::cout << "Ensure battery power is connected" << std::endl;
                        }
                    }
                } else if (command == "simulate") {
                    try {
                        m18.simulate();
                    } catch (const std::exception& e) {
                        std::cout << "Error during simulation: " << e.what() << std::endl;
                    }
                } else if (command == "high") {
                    try {
                        m18.high();
                        std::cout << "J2 is now high (20V)" << std::endl;
                    } catch (const std::exception& e) {
                        std::cout << "Error setting J2 high: " << e.what() << std::endl;
                    }
                } else if (command == "idle") {
                    try {
                        m18.idle();
                        std::cout << "J2 is now low (0V)" << std::endl;
                    } catch (const std::exception& e) {
                        std::cout << "Error setting J2 low: " << e.what() << std::endl;
                    }
                } else if (command.substr(0, 9) == "high_for ") {
                    try {
                        int seconds = std::stoi(command.substr(9));
                        m18.high_for(seconds);
                        std::cout << "J2 was high for " << seconds << " seconds" << std::endl;
                    } catch (const std::exception& e) {
                        std::cout << "Error: " << e.what() << std::endl;
                        std::cout << "Usage: high_for N" << std::endl;
                    } catch (...) {
                        std::cout << "Usage: high_for N" << std::endl;
                    }
                } else if (command == "help") {
                    std::cout << R"(Available commands:
  health              - Print simple health report on battery
  read_id             - Print all registers in labelled format
  simulate            - Simulate charger communication
  high                - Bring J2 pin high (20V)
  idle                - Pull J2 pin low (0V)
  high_for N          - Bring J2 high for N seconds then idle
  exit or quit        - Exit the program
)" << std::endl;
                } else if (!command.empty()) {
                    std::cout << "Unknown command. Type 'help' for commands." << std::endl;
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
