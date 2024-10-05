#include <cctype>
#include <iomanip>
#include <iostream>
#include <map>
#include <sstream>

/**
 * @brief Escapes JSON special characters in a string.
 *
 * This function takes a string and returns a new string with all the JSON special characters
 * properly escaped according to the JSON specification.
 *
 * @param s The input string to escape.
 * @return std::string The escaped string.
 */
std::string escape_to_json(const std::string &s) {
    std::ostringstream oss;
    for (const char c : s) {
        switch (c) {
        case '"':
            oss << "\\\"";
            break;
        case '\\':
            oss << "\\\\";
            break;
        case '\b':
            oss << "\\b";
            break;
        case '\f':
            oss << "\\f";
            break;
        case '\n':
            oss << "\\n";
            break;
        case '\r':
            oss << "\\r";
            break;
        case '\t':
            oss << "\\t";
            break;
        default:
            if (std::isprint(c))
                oss << c;  // If the character is printable, add it as is
            else
                oss << "\\u"  // Otherwise, output as a Unicode escape sequence
                    << std::hex
                    << std::setw(4)
                    << std::setfill('0')
                    << static_cast<unsigned>(c);
        }
    }
    return oss.str();
}

/**
 * @brief Serializes a map of strings to a JSON string.
 *
 * This function takes a map of strings and serializes it into a JSON formatted string.
 *
 * @param m The map to serialize.
 * @return std::string The JSON formatted string.
 */
std::string serialize_to_json(const std::map<std::string, std::string> &m) {
    std::ostringstream oss;
    oss << "{";
    bool is_first = true;
    for (const auto &[key, value] : m) {
        if (is_first)
            is_first = false;
        else
            oss << ",";

        oss << "\"" << escape_to_json(key) << "\""
            << ":"
            << "\"" << escape_to_json(value) << "\"";
    }
    oss << "}";
    return oss.str();
}

/**
 * @brief Main entry point of the program.
 *
 * This is the main function that demonstrates the serialization of a map to a JSON string.
 *
 * @return int The exit code of the program.
 */
int main() {
    std::map<std::string, std::string> m = {
        {"sjdf", "sk\ndjfksdf"},
        {"hsdhf", "sjdgfjsdhgfj sdgfjsd"},
        {"sjdh", "sjdhfg\x01sjdf sdf"},
        {"uery", "uerytjfsd"},
        {"jsdkf", "123456"},
        {"sdhs", "ksjhdfksf"},
    };

    std::string json = serialize_to_json(m);
    std::cout << json << std::endl;
    return 0;
}
