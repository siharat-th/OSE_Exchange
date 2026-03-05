#ifndef CFE_RESET_SESSION_HPP
#define CFE_RESET_SESSION_HPP

#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <filesystem>
#include <algorithm>
#include <spdlog/spdlog.h>

namespace fs = std::filesystem;

// The key to search for in the config file
constexpr const char* STATE_FILE_KEY = "BOE3.StateFileDirectory";

/**
 * Reads the config file and extracts the directory path from the
 * line with the key 'BOE3.StateFileDirectory'. It handles spaces around '='.
 *
 * @param config_file Path to the configuration file
 * @param state_dir Output parameter for the state directory path
 * @return true if state directory was found, false otherwise
 */
inline bool get_state_file_directory(const std::string& config_file, std::string& state_dir) {
    spdlog::debug("Attempting to read config file: {}", config_file);

    std::ifstream file(config_file);
    if (!file.is_open()) {
        spdlog::error("Config file '{}' not found.", config_file);
        return false;
    }

    std::string line;
    while (std::getline(file, line)) {
        // Trim whitespace
        line.erase(0, line.find_first_not_of(" \t\r\n"));
        line.erase(line.find_last_not_of(" \t\r\n") + 1);

        // Skip blank lines and comments
        if (line.empty() || line[0] == '#') {
            continue;
        }

        spdlog::debug("Config line read: {}", line);

        // Look for '=' separator
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = line.substr(0, pos);
            std::string value = line.substr(pos + 1);

            // Trim key and value
            key.erase(0, key.find_first_not_of(" \t"));
            key.erase(key.find_last_not_of(" \t") + 1);
            value.erase(0, value.find_first_not_of(" \t"));
            value.erase(value.find_last_not_of(" \t") + 1);

            if (key == STATE_FILE_KEY) {
                state_dir = value;
                spdlog::info("Found state file directory: {}", state_dir);
                return true;
            }
        }
    }

    spdlog::error("Could not find '{}' setting in config file.", STATE_FILE_KEY);
    return false;
}

/**
 * Reads the target file, updates the 'Negotiated', 'MsgSeqIn', and 'MsgSeqOut' lines (if found),
 * and writes the modified content back to the file.
 *
 * @param file_path Path to the file to update
 * @param new_value New value for Negotiated field ("True" or "False")
 * @return true if the file was changed, false otherwise
 */
inline bool update_negotiated_in_file(const std::string& file_path, const std::string& new_value) {
    spdlog::debug("Processing file: {} to set Negotiated={}", file_path, new_value);

    // Read file
    std::ifstream file(file_path);
    if (!file.is_open()) {
        spdlog::error("Error reading {}", file_path);
        return false;
    }

    std::vector<std::string> lines;
    std::string line;
    while (std::getline(file, line)) {
        lines.push_back(line);
    }
    file.close();

    spdlog::debug("Read {} lines from {}", lines.size(), file_path);

    // Update lines
    bool changed = false;
    std::vector<std::string> updated_lines;

    for (const auto& line : lines) {
        if (line.rfind("Negotiated=", 0) == 0) {
            spdlog::debug("Found 'Negotiated' line: {}", line);
            updated_lines.push_back("Negotiated=" + new_value);
            changed = true;
        } else if (line.rfind("MsgSeqIn=", 0) == 0) {
            spdlog::debug("Found 'MsgSeqIn' line: {}", line);
            updated_lines.push_back("MsgSeqIn=0");
            changed = true;
        } else if (line.rfind("MsgSeqOut=", 0) == 0) {
            spdlog::debug("Found 'MsgSeqOut' line: {}", line);
            updated_lines.push_back("MsgSeqOut=0");
            changed = true;
        } else {
            updated_lines.push_back(line);
        }
    }

    // Write back if changed
    if (changed) {
        std::ofstream out_file(file_path);
        if (!out_file.is_open()) {
            spdlog::error("Error writing to {}", file_path);
            return false;
        }

        for (const auto& line : updated_lines) {
            out_file << line << "\n";
        }
        out_file.close();

        spdlog::info("Updated file successfully: {}", file_path);
    } else {
        spdlog::info("No 'Negotiated' setting found in {}; no changes made.", file_path);
    }

    return changed;
}

/**
 * Main function to reset session files.
 *
 * @param value New value for Negotiated ("True" or "False")
 * @param config_file Path to the config file (default: "cmeIL3.conf")
 * @param target_file Optional: specific file to update (empty string = update all files)
 * @param verbose Enable verbose logging
 * @return true if successful, false otherwise
 */
inline bool reset_session(const std::string& value,
                          const std::string& config_file = "cmeIL3.conf",
                          const std::string& target_file = "",
                          bool verbose = false) {
    // Set up logging
    spdlog::set_pattern("%H:%M:%S.%f %^[%l]%$ %v");
    if (verbose) {
        spdlog::set_level(spdlog::level::debug);
    } else {
        spdlog::set_level(spdlog::level::info);
    }

    spdlog::info("ResetSession started.");

    // Validate value
    if (value != "True" && value != "False") {
        spdlog::error("Invalid value '{}'. Must be 'True' or 'False'.", value);
        return false;
    }

    // Get state directory from config
    std::string state_dir;
    if (!get_state_file_directory(config_file, state_dir)) {
        spdlog::error("Could not find '{}' setting in {}. Exiting.", STATE_FILE_KEY, config_file);
        return false;
    }

    spdlog::info("Using state directory: {}", state_dir);

    // Update specific target file if provided
    if (!target_file.empty()) {
        fs::path target_path;
        if (fs::path(target_file).is_absolute()) {
            target_path = target_file;
        } else {
            target_path = fs::path(state_dir) / target_file;
        }

        spdlog::debug("Resolved target file path: {}", target_path.string());

        if (!fs::exists(target_path) || !fs::is_regular_file(target_path)) {
            spdlog::error("Target file '{}' does not exist. Exiting.", target_path.string());
            return false;
        }

        if (update_negotiated_in_file(target_path.string(), value)) {
            spdlog::info("Updated 'Negotiated' in file: {}", target_path.string());
        } else {
            spdlog::info("No 'Negotiated' setting found or file not updated: {}", target_path.string());
        }
    } else {
        // Update all files in the state directory
        if (!fs::exists(state_dir) || !fs::is_directory(state_dir)) {
            spdlog::error("State file directory '{}' does not exist or is not a directory. Exiting.", state_dir);
            return false;
        }

        std::vector<std::string> updated_files;
        spdlog::debug("Listing files in state directory...");

        for (const auto& entry : fs::directory_iterator(state_dir)) {
            if (entry.is_regular_file()) {
                std::string file_path = entry.path().string();
                spdlog::debug("Processing file: {}", file_path);

                if (update_negotiated_in_file(file_path, value)) {
                    updated_files.push_back(file_path);
                }
            }
        }

        if (!updated_files.empty()) {
            spdlog::info("Updated 'Negotiated' in the following files:");
            for (const auto& f : updated_files) {
                spdlog::info("  {}", f);
            }
        } else {
            spdlog::info("No files were updated. 'Negotiated' setting was not found in any file.");
        }
    }

    spdlog::info("ResetSession finished.");
    return true;
}

#endif // CFE_RESET_SESSION_HPP
