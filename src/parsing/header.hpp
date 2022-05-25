#pragma once

#include <istream>

#include "status.hpp"
#include "../types.hpp"

Status parse_header(std::istream& in, Header& header);
