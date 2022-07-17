#pragma once

#include <charconv>
#include <iostream>
#include <optional>
#include <string>
#include <sstream>

struct CmdOptions {
    std::optional<std::string> input_filename;
    std::string output_filename = "out.svg";

    std::optional<size_t> width;
    std::optional<size_t> height;

    bool verbose = false;
    bool adjust_colors = false;
};

const std::string options_help = R"(
Options
  -o <file>          The filename of the output image.
                     Default is 'out.svg'.
  -v                 Enables verbose output.
  -h, --help         Print usage and exit.
  -w <width>
  --width <width>    The maximum width of the image.
                     The actual width can be smaller.
  -h <height>
  --height <height>  The maximum height of the image.
                     The actual height can be smaller.
  -a,
  --adjust-colors    Compute colors based on the maximum occupancy of blocks
                     instead of based on block capacity.
)";

void print_usage(const std::string& executable_name) {
    std::cout << "Usage: " << executable_name << " <input_file.mtx> -o <output_file.svg>\n";
    std::cout << options_help;
}

std::optional<size_t> parse_integer_argument(std::string_view arg_name, std::string_view arg_val) {
    std::string arg_string(arg_val);

    size_t end = -1;
    uint32_t val = -1;

    try {
        val = std::stol(arg_string, &end);
    } catch (const std::invalid_argument& /*ex*/) {
        std::cerr << "Error: Invalid value '" << arg_val << "'"
                  << " provided for the '" << arg_name << "' option.\n";
        return std::nullopt;
    } catch (const std::out_of_range& /*ex*/) {
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
        } else if (arg == "-h" || arg == "--help") {
            print_usage(argv[0]);
            return std::nullopt;
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
            if (opts.input_filename) {
                std::cout << "Error: Multiple input files specified: '" << *opts.input_filename << "' and '" << arg << "'.\n";
                return std::nullopt;
            }
            opts.input_filename = arg;
        }
    }

    return opts;
}
