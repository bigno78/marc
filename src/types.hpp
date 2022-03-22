#pragma once

enum class Symmetry {
    non_symmetric,
    symmetric,
    skew_symmetric
};

struct Header {
    bool is_pattern = false;
    Symmetry symmetry = Symmetry::non_symmetric;
};
