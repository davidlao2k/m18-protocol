#ifndef DATA_TABLES_HPP
#define DATA_TABLES_HPP

#include <cstdint>
#include <vector>
#include <string>
#include <map>

// Data matrix for reading raw registers
extern const std::vector<std::vector<uint16_t>> DATA_MATRIX;

// Battery lookup table (type -> capacity, model name)
extern const std::map<std::string, std::pair<float, std::string>> BATTERY_LOOKUP;

// Data ID table with register information
extern const std::vector<std::vector<std::string>> DATA_ID;

#endif // DATA_TABLES_HPP
