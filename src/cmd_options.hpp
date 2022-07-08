#pragma once

#include <charconv>
#include <iostream>
#include <optional>
#include <string>
#include <sstream>

struct CmdOptions {
    std::string input_filename;
    std::string output_filename = "out.svg";

    std::optional<size_t> width;
    std::optional<size_t> height;

    bool verbose = false;
    bool adjust_colors = false;
};

std::optional<size_t> parse_integer_argument(std::string_view arg_name, std::string_view arg_val) {
    std::string arg_string(arg_val);
    
    size_t end = -1;
    uint32_t val = -1;
    
    try {
        val = std::stol(arg_string, &end);
    } catch (const std::invalid_argument& ex) {
        std::cerr << "Error: Invalid value '" << arg_val << "'"
                  << " provided for the '" << arg_name << "' option.\n";
        return std::nullopt;
    } catch (const std::out_of_range& ex) {
        std::cerr << "Error: Whoa! The value '" << arg_val << "' provided for the "
                  << "'" << arg_name << "' option is a bit too large buddy.\n";
        return std::nullopt;
    }

    if (end != arg_string.size()) {
        std::cerr << "Error: Invalid value '" << arg_val << "'"
                  << " provided for the '" << arg_name << "' option.\n";
        return std::nullopt;
    }
    
    return val;
}

std::optional<CmdOptions> parse_args(int argc, char** argv) {
    CmdOptions opts;

    for (int i = 1; i < argc; ++i) {
        std::string_view arg(argv[i]);
        if (arg == "-o") {
            if (i >= argc - 1) {
                std::cout << "Error: -o needs a value";
                return std::nullopt;
            }
            opts.output_filename = argv[++i];
        } else if (arg == "-v" || arg == "--verbose") {
            opts.verbose = true;
        } else if (arg == "-w" || arg == "--width") {
            if (i >= argc - 1) {
                std::cerr << "Error: No value specified for '" << arg << "'.\n";
                return std::nullopt;
            }
            i++;
            opts.width = parse_integer_argument(argv[i - 1], argv[i]);
            if (!opts.width) {
                return std::nullopt;
            }
        } else if (arg == "-h" || arg == "--height") {
            if (i >= argc - 1) {
                std::cerr << "Error: No value specified for '" << arg << "'.\n";
                return std::nullopt;
            }
            i++;
            opts.height = parse_integer_argument(argv[i - 1], argv[i]);
            if (!opts.height) {
                return std::nullopt;
            }
        } else if (arg == "-a" || arg == "--adjust-colors") {
            opts.adjust_colors = true;
        } else {
            if (!opts.input_filename.empty()) {
                std::cout << "Error: Multiple input files specified: '" << opts.input_filename << "' and '" << arg << "'.\n";
                return std::nullopt;
            }
            opts.input_filename = arg;
        }
    }

    if (opts.input_filename.empty()) {
        std::cout << "Error: No input file specified.\n";
        return std::nullopt;
    }

    return opts;
}
