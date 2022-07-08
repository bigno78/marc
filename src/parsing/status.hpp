#pragma once

#include <string>


struct Status {
    bool is_error;
    std::string error_message;
    size_t line;
    size_t col;

    operator bool() const {
        return !is_error;
    }

    static Status error(const std::string& msg, size_t line, size_t col) {
        return { true, msg, line, col };
    }

    static Status success() {
        return { false };
    }
};
