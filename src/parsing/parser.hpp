#pragma once

#include "../types.hpp"

#include <fstream>


struct Status {
    bool is_error;
    std::string error_message;  
    size_t line;
    size_t col;

    static Status error(const std::string& msg, size_t line, size_t col) {
        return { true, msg, line, col };
    }

    static Status success() {
        return { false };
    }

    operator bool() const {
        return !is_error;
    }
};


Status parse_header(std::ifstream& in, Header& header);
