#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <iostream>
#include <assert.h>

class ConfigReader {
public:
    void read(const std::string& fname) {
        this->filename = fname;
        std::ifstream infile(fname);
        assert(infile.is_open());
        std::string line;

        while (std::getline(infile, line)) {
            // Store the original line
            lines.push_back(line);
            std::istringstream iss(line);
            std::string key, value;
            // Attempt to read key value pair
            if (iss >> key >> value) if (key[0] != '#') dict[key] = value;// Ignore lines that start with a comment indicator, such as '#'
        }

        infile.close();
    }

    void writeBack() {
        std::ofstream outfile(filename, std::ofstream::trunc);
        assert(outfile.is_open());

        for (const auto& line : lines) {
            std::istringstream iss(line);
            std::string key, maybe_value;
            if (iss >> key && dict.find(key) != dict.end()) outfile << key << " " << dict[key] << std::endl; // Write the key and updated value
            else outfile << line << std::endl; // Write the line unchanged
        }

        outfile.close();
    }

    int getInt(const std::string& key) {
        if (dict.find(key) != dict.end()) return std::stoi(dict[key]);
        throw std::runtime_error("Key not found: " + key);
    }

    double getDouble(const std::string& key) {
        if (dict.find(key) != dict.end()) return std::stod(dict[key]); 
        throw std::runtime_error("Key not found: " + key);
    }

    std::string getString(const std::string& key) {
        if (dict.find(key) != dict.end()) return dict[key];
        throw std::runtime_error("Key not found: " + key);
    }

    void setInt(const std::string& key, int value) {
        dict[key] = std::to_string(value);
        updateLine(key, std::to_string(value));
    }

    void setDouble(const std::string& key, double value) {
        dict[key] = std::to_string(value);
        updateLine(key, std::to_string(value));
    }

    void setString(const std::string& key, const std::string& value) {
        dict[key] = value;
        updateLine(key, value);
    }

private:
    std::unordered_map<std::string, std::string> dict;
    std::vector<std::string> lines;
    std::string filename;

    void updateLine(const std::string& key, const std::string& value) {
        for (auto& line : lines) {
            std::istringstream iss(line);
            std::string current_key;
            if (iss >> current_key && current_key == key) { line = key + " " + value; return; }// Update the line with the new value
        }
        // If the key is not found in the existing lines, add a new line for it
        lines.push_back(key + " " + value);
    }
};


