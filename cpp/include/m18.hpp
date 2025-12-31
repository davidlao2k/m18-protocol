#ifndef M18_HPP
#define M18_HPP

#include <cstdint>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <chrono>

// Forward declaration for serial port
class SerialPort;

// Data structures for battery information
struct DataMatrixEntry {
    uint8_t addr_h;
    uint8_t addr_l;
    uint16_t length;
};

struct DataIdEntry {
    uint16_t addr;
    uint16_t length;
    std::string type;  // "uint", "date", "ascii", "sn", "adc_t", "dec_t", "cell_v", "hhmmss"
    std::string label;
};

struct CellVoltages {
    std::vector<uint16_t> voltages;  // 5 cells
};

struct BatteryHealth {
    std::string type;
    std::string model;
    std::string serial;
    std::string manufacture_date;
    int days_since_first_charge;
    int days_since_last_use;
    int days_since_last_charge;
    float pack_voltage;
    CellVoltages cell_voltages;
    float cell_imbalance;
    float temperature;
    int charge_count_redlink;
    int charge_count_dumb;
    int charge_count_total;
    std::string total_charge_time;
    std::string idle_on_charger_time;
    int low_voltage_charges;
    float total_discharge_ah;
    float discharge_cycles;
    int discharge_to_empty;
    int overheat_events;
    int overcurrent_events;
    int low_voltage_events;
    int low_voltage_bounce;
    std::string total_time_on_tool;
    std::vector<std::pair<std::string, int>> current_buckets;  // amplitude range and seconds
};

class M18 {
public:
    // Constants
    static constexpr uint8_t SYNC_BYTE = 0xAA;
    static constexpr uint8_t CAL_CMD = 0x55;
    static constexpr uint8_t CONF_CMD = 0x60;
    static constexpr uint8_t SNAP_CMD = 0x61;
    static constexpr uint8_t KEEPALIVE_CMD = 0x62;
    
    static constexpr uint16_t CUTOFF_CURRENT = 300;
    static constexpr uint16_t MAX_CURRENT = 6000;

    M18(const std::string& port = "");
    ~M18();

    // Connection management
    bool connect(const std::string& port);
    void disconnect();
    bool is_connected() const;

    // Low-level commands
    bool reset();
    std::vector<uint8_t> configure(uint8_t state);
    std::vector<uint8_t> get_snapchat();
    std::vector<uint8_t> keepalive();
    std::vector<uint8_t> calibrate();
    
    // High-level diagnostics
    BatteryHealth health(bool force_refresh = true);
    void read_id(std::vector<int> id_array = {}, bool force_refresh = true, const std::string& output = "label");
    std::vector<uint8_t> read_all();
    void read_all_spreadsheet();

    // Interactive/test functions
    void simulate(int duration_seconds = -1);
    void high();
    void idle();
    void high_for(int duration_seconds);
    
    // Debugging
    void brute(uint8_t addr_msb, uint8_t addr_lsb, uint16_t length = 0xFF, uint8_t command = 0x01);
    void full_brute(uint16_t start = 0, uint16_t stop = 0xFFFF, uint16_t length = 0xFF);
    void debug(uint8_t a, uint8_t b, uint8_t c, uint16_t length);
    void try_cmd(uint8_t cmd, uint8_t msb, uint8_t lsb, uint8_t length, uint16_t ret_len = 0);
    
    void write_message(const std::string& message);
    void submit_form();
    
    // Debugging output control
    bool print_tx = false;
    bool print_rx = false;
    
    // Port selection (public for CLI use)
    std::string select_port();

private:
    std::unique_ptr<SerialPort> port_;
    bool connected_;
    uint8_t acc_;
    
    // Private helper methods
    std::vector<uint8_t> cmd(uint8_t a, uint8_t b, uint8_t c, uint16_t length, uint8_t command = 0x01);
    void send(const std::vector<uint8_t>& command);
    void send_command(std::vector<uint8_t> command);
    std::vector<uint8_t> read_response(size_t size);
    
    std::vector<uint8_t> add_checksum(std::vector<uint8_t> lsb_command);
    uint16_t checksum(const std::vector<uint8_t>& payload);
    uint8_t reverse_bits(uint8_t byte);
    void update_acc();
    
    // Data parsing helpers
    std::string bytes_to_date_string(const std::vector<uint8_t>& data);
    std::string bytes_to_hhmmss(const std::vector<uint8_t>& data);
    float calculate_temperature(uint16_t adc_value);
    CellVoltages extract_cell_voltages(const std::vector<uint8_t>& data);
};

#endif // M18_HPP
