#pragma once

#include <string>

inline int to_lower(unsigned char a) {
    return tolower(a);
}

inline bool case_insensitive_eq(const std::string& lhs, const std::string& rhs) {
    if (lhs.size() != rhs.size())
        return false;

    for (size_t i = 0; i < lhs.size(); ++i)
    {
        if (to_lower(lhs[i]) != to_lower(rhs[i]))
        {
            return false;
        }
    }

    return true;
}

template<typename T>
T div_ceil(T a, T b) {
    return (a + b - 1)/b;
}
