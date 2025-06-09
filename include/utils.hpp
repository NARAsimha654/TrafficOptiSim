#ifndef UTILS_HPP
#define UTILS_HPP

#include <vector>
#include <string>
#include <sstream> // Required for std::stringstream
#include <fstream> // Required for std::ifstream

// Namespace for utility functions
namespace Utils
{

    // MOVED: The CsvRow struct is now inside the Utils namespace
    // This allows you to call it using Utils::CsvRow from other files.
    struct CsvRow
    {
        std::vector<std::string> fields;
    };

    // Parses a CSV file into a vector of CsvRow objects
    // Each CsvRow contains a vector of strings (fields)
    std::vector<CsvRow> parse_csv(const std::string &filepath, char delimiter = ',');

    // Helper function to trim whitespace from both ends of a string
    std::string trim_whitespace(const std::string &str);

    // Converts a string to an integer, with error handling
    // Returns true on success, false on failure. Value is stored in out_value.
    bool string_to_int(const std::string &str, int &out_value);

    // Converts a string to a double, with error handling
    // Returns true on success, false on failure. Value is stored in out_value.
    bool string_to_double(const std::string &str, double &out_value);

} // namespace Utils

#endif // UTILS_HPP