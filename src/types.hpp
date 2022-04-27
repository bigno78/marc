#pragma once

#include <cstddef>

enum class Format {
    coordinate,
    array
};

enum class Type {
    real,
    integer,
    complex,
    pattern
};

enum class Symmetry {
    general,
    symmetric,
    skew_symmetric,
    hermitian
};

struct Header {
    Format format;
    Type type;
    Symmetry symmetry;

    std::size_t rows;
    std::size_t cols;
    std::size_t entries;
};
