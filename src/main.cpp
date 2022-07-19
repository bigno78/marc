#include <iostream>
#include <sstream>
#include <fstream>
#include <tuple>
#include <string>
#include <vector>
#include <string_view>
#include <map>

#include "parsing/parser.hpp"

#include "utils.hpp"
#include "types.hpp"
#include "grid.hpp"
#include "cmd_options.hpp"

#include "drawing/draw.hpp"
#include "drawing/generic.hpp"
#include "drawing/stb_image.hpp"
#include "drawing/svg.hpp"

void print_parsing_error(const Status& status) {
    std::cerr << "Error:" << status.line << ":" << status.col << ": " << status.error_message << "\n";
}

void print_matrix_info(const Header& header) {
    std::cout << "Matrix parameters:\n";
    std::cout << "    rows:     " << header.rows << "\n";
    std::cout << "    cols:     " << header.cols << "\n";
    std::cout << "    entries:  " << header.entries << " (these are only entries in the file and don't account for symmetry)\n";
    std::cout << "    symmetry: ";
    switch (header.symmetry) {
        case Symmetry::general:
            std::cout << "general (no symmetry)";
            break;
        case Symmetry::symmetric:
            std::cout << "symmetric";
            break;
        case Symmetry::skew_symmetric:
            std::cout << "skew-symmetric";
            break;
        case Symmetry::hermitian:
            std::cout << "hermitian";
            break;
    }
    std::cout << "\n\n";
}

std::string get_image_extension(ImageFormat format) {
    static std::map<ImageFormat, std::string> extensions = {
        { ImageFormat::png, ".png" },
        { ImageFormat::jpg, ".jpg" },
        { ImageFormat::bmp, ".bmp" },
        { ImageFormat::tga, ".tga" },
        { ImageFormat::svg, ".svg" },
    };
    return extensions.at(format);
}

ImageConfig init_image_config(const Header& header, const CmdOptions& opts) {
    ImageConfig config;

    if (opts.width && opts.height) {
        config.viewport_width = opts.width.value();
        config.viewport_height = opts.height.value();
    } else if(opts.width) {
        config.viewport_width = opts.width.value();
        config.viewport_height = (size_t) ((config.viewport_width*header.rows)/float(header.cols));
    } else if (opts.height) {
        config.viewport_height = opts.height.value();
        config.viewport_width = (size_t) ((config.viewport_height*header.cols)/float(header.rows));
    }

    if (!opts.output_filename) {
        config.path = "out" + get_image_extension(opts.image_format);
    } else {
        config.path = opts.output_filename.value();
    }

    config.adjust_colors = opts.adjust_colors;
    config.format = opts.image_format;

    return config;
}

Grid make_grid(const Header& header, const ImageConfig& config, const CmdOptions& opts) {
    size_t max_blocks_x = (config.viewport_width - 2*config.border_size) / config.block_size;
    size_t max_blocks_y = (config.viewport_height - 2*config.border_size) / config.block_size;

    Grid grid(header, max_blocks_y, max_blocks_x);

    if (opts.verbose) {
        std::cout << "Grid parameters:\n";
        std::cout << "    rows:           " << grid.rows() << "\n";
        std::cout << "    cols:           " << grid.cols() << "\n";
        std::cout << "    block size:     " << grid.block_size() << "\n";
        std::cout << "    block capacity: " << grid.block_capacity() << "\n";
        std::cout << "\n";
    }

    return grid;
}


void draw_grid(const Grid& grid, ImageConfig& image_config, const CmdOptions& opts) {
    image_config.width = grid.cols() * image_config.block_size + 2*image_config.border_size;
    image_config.height = grid.rows() * image_config.block_size + 2*image_config.border_size;

    if (opts.verbose) {
        std::cout << "Image parameters:\n";
        std::cout << "   max_width:     " << image_config.viewport_width << "\n";
        std::cout << "   max_height:    " << image_config.viewport_height << "\n";
        std::cout << "   width:         " << image_config.width << "\n";
        std::cout << "   height:        " << image_config.height << "\n";
        std::cout << "   adjust colors: " << (image_config.adjust_colors ? "on" : "off") << "\n";
        std::cout << "\n";
    }

    if (image_config.format == ImageFormat::svg) {
        GenericDrawer<SvgImage> drawer;
        drawer(grid, image_config);
    } else {
        GenericDrawer<StbImage> drawer;
        drawer(grid, image_config);
    }
}


int main(int argc, char** argv) {
    std::optional<CmdOptions> opts = parse_args(argc, argv);

    if (!opts) {
        return EXIT_FAILURE;
    }

    std::optional<std::ifstream> input_file;
    if (opts->input_filename) {
        input_file = std::ifstream(opts->input_filename.value());
    } else {
        std::ios_base::sync_with_stdio(false);
        std::cin.tie(0);
    }

    std::istream& input = input_file ? *input_file : std::cin;

    Header header;
    auto status = parse_header(input, header);
    if (!status) {
        print_parsing_error(status);
        return EXIT_FAILURE;
    }

    if (header.format == Format::array) {
        std::cerr << "Array format is not supported. Only sparse matrices (coordinate format) are supported.\n";
        return EXIT_FAILURE;
    }

    if (opts->verbose) {
        print_matrix_info(header);
    }

    ImageConfig image_config = init_image_config(header, *opts);
    Grid grid = make_grid(header, image_config, *opts);

    status = read_entries_custom(input, header, grid);
    if (!status) {
        print_parsing_error(status);
        return EXIT_FAILURE;
    }

    if (opts->verbose) {
        std::cout << "Entries processed: " << grid.entries() << "\n\n";
    }

    draw_grid(grid, image_config, *opts);

    return EXIT_SUCCESS;
}