#include "utils.hpp"
#include <algorithm> // For std::remove_if for trim_whitespace, std::stoi, std::stod
#include <cctype>    // For std::isspace
#include <iostream>  // For potential error messages (optional)

namespace Utils {

std::string trim_whitespace(const std::string& str) {
    std::string result = str;
    // Remove leading whitespace
    result.erase(result.begin(), std::find_if(result.begin(), result.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    }));
    // Remove trailing whitespace
    result.erase(std::find_if(result.rbegin(), result.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), result.end());
    return result;
}

std::vector<CsvRow> parse_csv(const std::string& filepath, char delimiter) {
    std::vector<CsvRow> data;
    std::ifstream file(filepath);

    if (!file.is_open()) {
        // Handle error: file could not be opened
        // std::cerr << "Error: Could not open CSV file: " << filepath << std::endl;
        return data; // Return empty data
    }

    std::string line;
    while (std::getline(file, line)) {
        CsvRow row;
        std::stringstream line_stream(line);
        std::string field;

        while (std::getline(line_stream, field, delimiter)) {
            row.fields.push_back(trim_whitespace(field));
        }
        data.push_back(row);
    }

    file.close();
    return data;
}

bool string_to_int(const std::string& str, int& out_value) {
    try {
        out_value = std::stoi(trim_whitespace(str));
        return true;
    } catch (const std::invalid_argument& ia) {
        // std::cerr << "Invalid argument for stoi: " << str << std::endl;
        return false;
    } catch (const std::out_of_range& oor) {
        // std::cerr << "Out of range for stoi: " << str << std::endl;
        return false;
    }
}

bool string_to_double(const std::string& str, double& out_value) {
    try {
        out_value = std::stod(trim_whitespace(str));
        return true;
    } catch (const std::invalid_argument& ia) {
        // std::cerr << "Invalid argument for stod: " << str << std::endl;
        return false;
    } catch (const std::out_of_range& oor) {
        // std::cerr << "Out of range for stod: " << str << std::endl;
        return false;
    }
}

} // namespace Utils
