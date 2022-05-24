#pragma once

#include <string>
#include <istream>


/**
 * Extracts an integer from `str` starting from index `i`
 * assigns it into `val` and assigns the first unprocessed index to `end`.
 * 
 * If the integer doesn't fit into size_t returns false,
 * otherwise returns true.
 * 
 * Doesn't skip any leading whitespace.
 */
bool read_int(const std::string& str, size_t i, size_t& end, size_t& val) {
    constexpr size_t max_val = size_t(-1);
    constexpr size_t risky_val = max_val/10;
    constexpr size_t max_digit = max_val % 10;
    
    size_t res = 0;
    while(i < str.size() && isdigit(str[i])) {
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
bool process_entry(const std::string& str, DataCollector& collector) {
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

    size_t row;
    size_t end;
    if (!read_int(str, i, end, row)) {
        return false;
    }
    i = end;

    while (i < str.size() && isspace(str[i])) {
        ++i;
    }

    if (i >= str.size() || !isdigit(str[i])) {
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
void read_data_from_stream(std::istream& in, DataCollector& collector) {
    std::string line;

    while (std::getline(in, line)) {
        if (!process_entry(line, collector)) {
            return;
        }
    }
}
