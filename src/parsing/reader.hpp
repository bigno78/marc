#pragma once

#include <string>
#include <istream>

/**
 * Extracts and integers from `str` starting from index `i`.
 * If the integer doesn't fit into size_t return false,
 * othervise returns true.
 */
bool read_integer(const std::string& str, size_t i, size_t& val, size_t& end) {
    size_t res = 0;
    
    while(i < str.size() && isdigit(str[i])) {
        size_t new_res = val*10 + (str[i] - '0');
        if (new_res < res) {
            return false;
        }
        ++i;
    }

    end = i;
    val = res;

    return true;
}

template<typename DataCollector>
bool read_entry(const std::string& str, DataCollector& collector) {
    size_t i = 0;

    while (i < str.size() && isspace(str[i])) {
        ++i;
    }

    if (i >= str.size()) {
        return true;
    }

    if (!isdigit(str[i])) {
        return false;
    }

    size_t row = 0;
    while(i < str.size() && isdigit(str[i])) {
        row = row*10 + (str[i] - '0');
        ++i;
    }

    while (i < str.size() && isspace(str[i])) {
        ++i;
    }

    if (i >= str.size() || !isdigit(str[i])) {
        return false;
    }

    size_t col = 0;
    while(i < str.size() && isdigit(str[i])) {
        col = col*10 + (str[i] - '0');
        ++i;
    }

    collector.on_entry(row, col);

    return true;
}


template<typename DataCollector>
void read_data_from_stream(std::istream& in, DataCollector& collector) {
    std::string line;

    while (std::getline(in, line)) {
        if (!read_entry(line, collector)) {
            return;
        }
    }
}
