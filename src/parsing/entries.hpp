#pragma once

#include <fstream>
#include <string>

#include "reader.hpp"


bool is_digit(char c) {
    const unsigned x = c;
    return (x - '0') <= 9;
}

/**
 * Extracts an integer from `str` starting from index `i`
 * assigns it into `val` and assigns the first unprocessed index to `end`.
 * 
 * If the integer doesn't fit into size_t returns false,
 * otherwise returns true.
 * 
 * Doesn't skip any leading whitespace.
 */
bool read_int(const char* str, size_t i, size_t& end, size_t& val) {
    constexpr size_t max_val = size_t(-1);
    constexpr size_t risky_val = max_val/10;
    constexpr size_t max_digit = max_val % 10;
    
    size_t res = 0;
    while(is_digit(str[i])) {
        size_t d = str[i] - '0';
        if (res < risky_val || (res == risky_val && d <= max_digit)) {
            res = res*10 + d;
        } else {
            return false;
        }
        ++i;
    }

    end = i;
    val = res;

    return true;
}


template<typename DataCollector>
bool process_entry(const char* str, DataCollector& collector) {
    size_t i = 0;

    while (isspace(str[i])) {
        ++i;
    }

    if (str[i] == '\0') {
        return true;
    }

    if (!is_digit(str[i])) {
        return false;
    }

    size_t row;
    size_t end;
    if (!read_int(str, i, end, row)) {
        return false;
    }
    i = end;

    while (isspace(str[i])) {
        ++i;
    }

    if (str[i] == '\0' || !is_digit(str[i])) {
        return false;
    }

    size_t col;
    if (!read_int(str, i, end, col)) {
        return false;
    }

    collector.on_entry(row - 1, col - 1);

    return true;
}


template<typename DataCollector>
void read_entries_buffered(std::ifstream input, DataCollector& collector) {
    FileReaderLine<64> in(std::move(input));
    std::string line;
    while (in.getline(line)) {
        process_entry(line.c_str(), collector);
    }
}


template<typename DataCollector>
void read_entries_getline(std::ifstream input, DataCollector& collector) {
    std::string line;
    while (std::getline(input, line)) {
        if (!process_entry(line.c_str(), collector)) return;
    }
}


template<typename DataCollector>
void read_entries_custom(std::ifstream file, DataCollector& collector) {
    constexpr size_t buffer_size = 4096;
    std::array<char, buffer_size> buffer;

    std::array<char, 1025> line;
    size_t j = 0;

    while (true) {
        file.read(buffer.data(), buffer_size);
        size_t bytes_read = file.gcount();
        
        for (size_t i = 0; i < bytes_read; ++i) {
            if (j >= 1024) {
                std::cerr << "LONG LINE\n";
                return;
            }

            line[j++] = buffer[i];
            
            if (buffer[i] == '\n') {
                line[j] = '\0';
                process_entry(line.data(), collector);
                j = 0;
            }
        }

        if (!file) {
            break;
        }
    }
}
