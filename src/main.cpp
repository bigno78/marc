#include <iostream>
#include <sstream>
#include <fstream>
#include <tuple>
#include <string>
#include <vector>

#include "types.hpp"
#include "parser.hpp"
#include "grid.hpp"
#include "reader.hpp"
#include "utils.hpp"

#include "drawing/draw.hpp"
#include "drawing/svg.hpp"


struct cmd_options {
    std::string input_filename;
    std::string output_filename = "out.svg";

    bool verbose = false;
};

cmd_options parse_args(int argc, char** argv) {
    cmd_options opts;

    for (int i = 1; i < argc; ++i) {
        std::string_view arg(argv[i]);
        if (arg == "-o") {
            if (i >= argc - 1) {
                std::cout << "error: -o needs a value";
                std::exit(1);
            }
            opts.output_filename = argv[++i];
        } else if (arg == "-v") {
            opts.verbose = true;
        } else {
            if (!opts.input_filename.empty()) {
                std::cout << "error: multiple input files specified: '" << opts.input_filename << "' and '" << arg << "'\n";
                std::exit(1);
            }
            opts.input_filename = arg;
        }
    }

    if (opts.input_filename.empty()) {
        std::cout << "error: no input file specified\n";
        std::exit(1);
    }

    return opts;
}


Grid make_grid(const Header& header, const ImageConfig& config) {
    size_t max_blocks_x = (config.viewport_width - 2*config.border_size) / config.block_size;
    size_t max_blocks_y = (config.viewport_height - 2*config.border_size) / config.block_size;

    return Grid(header, max_blocks_y, max_blocks_x);
}


void draw_grid(const Grid& grid, ImageConfig& image_config) {
    image_config.width = grid.cols() * image_config.block_size + 2*image_config.border_size;
    image_config.height = grid.rows() * image_config.block_size + 2*image_config.border_size;

    SvgDrawer svg;
    svg(grid, image_config);
}


int main(int argc, char** argv) {
    cmd_options opts = parse_args(argc, argv);

    std::ifstream file(opts.input_filename);

    Header header;
    auto status = parse_header(file, header);

    if (!status) {
        std::cerr << "Error:" << status.line << ":" << status.col << ": " << status.error_message << "\n";
        return 1;
    }

    if (header.format == Format::array) {
        std::cerr << "Array format is not supported. Only sparse matrices (coordinate format) are supported.\n";
        return 1;
    }

    ImageConfig image_config;
    image_config.path = opts.output_filename;

    Grid grid = make_grid(header, image_config);
    
    if (opts.verbose) {
        std::cout << "Matrix of size " << header.rows << " x " << header.cols << " with " << header.entries << " entries\n";

        std::cout << "Grid: size " << grid.rows() << " x " << grid.cols();
        std::cout << " with " << grid.block_size() << " x " << grid.block_size() << " blocks";
        std::cout << " of capacity " << grid.block_capacity() << "\n";

        std::cout << "Image: size " << image_config.width << " x " << image_config.height;
        std::cout << " with " << image_config.block_size << " x " << image_config.block_size << " blocks\n";
    }
   
    read_data_from_stream(file, grid);

    draw_grid(grid, image_config);    
}