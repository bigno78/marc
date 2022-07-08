#pragma once

#include <fstream>
#include <string>

#include "parsing/status.hpp"
#include "grid.hpp"
#include "types.hpp"


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
Status read_int(const char* str, size_t i, size_t& end, size_t& val) {
    size_t start = i;
    
    constexpr size_t max_val = size_t(-1);
    constexpr size_t risky_val = max_val/10;
    constexpr size_t max_digit = max_val % 10;

    size_t res = 0;
    while(is_digit(str[i])) {
        size_t d = str[i] - '0';
        if (res < risky_val || (res == risky_val && d <= max_digit)) {
            res = res*10 + d;
        } else {
            return Status::error("Value is too large and would cause overflow.", -1, start + 1);
        }
        ++i;
    }

    end = i;
    val = res;

    return Status::success();
}


Status process_entry(const char* str, const Header& header, Grid& grid) {
    size_t i = 0;

    while (isspace(str[i])) {
        ++i;
    }

    if (str[i] == '\0') {
        return Status::success();
    }

    if (!is_digit(str[i])) {
        return Status::error("Unexpected character. Expected row index.", -1, i + 1);
    }

    size_t start = i;
    size_t row;
    size_t end;
    auto status = read_int(str, i, end, row);
    if (!status) {
        return status;
    }
    i = end;

    if (row > header.rows) {
        return Status::error("Row index out of bounds.", -1, start + 1);
    }

    while (isspace(str[i])) {
        ++i;
    }

    if (str[i] == '\0' || !is_digit(str[i])) {
        return Status::error("Unexpected character. Expected column index.", -1, i + 1);
    }

    start = i;
    size_t col;
    status = read_int(str, i, end, col);
    if (!status) {
        return status;
    }

    if (col > header.cols) {
        return Status::error("Col index out of bounds.", -1, start + 1);
    }

    grid.on_entry(row - 1, col - 1);

    return Status::success();
}


Status read_entries_getline(std::ifstream& input, const Header& header, Grid& grid) {
    size_t line_no = header.size + 1;
    std::string line;

    while (std::getline(input, line)) {
        auto status = process_entry(line.c_str(), header, grid);
        if (!status) {
            status.line = line_no + 1;
            return status;
        }
        ++line_no;
    }

    return Status::success();
}


Status read_entries_custom(std::ifstream& file, const Header& header, Grid& grid) {
    constexpr size_t buffer_size = 4096;
    std::array<char, buffer_size> buffer = { 0 };

    std::array<char, 1025> line = { 0 };
    size_t j = 0;
    size_t line_no = header.size + 1;

    while (true) {
        file.read(buffer.data(), buffer_size);
        size_t bytes_read = file.gcount();

        for (size_t i = 0; i < bytes_read; ++i) {
            if (j >= 1024) {
                return Status::error("Line is too long (over 1024 chars).", line_no + 1, 1);
            }

            line[j++] = buffer[i];

            if (buffer[i] == '\n') {
                line[j] = '\0';
                j = 0;

                auto status = process_entry(line.data(), header, grid);
                if (!status) {
                    status.line = line_no + 1;
                    return status;
                }

                ++line_no;
            }
        }

        if (!file) {
            break;
        }
    }

    return Status::success();
}
