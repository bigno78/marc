#pragma once

#include "draw.hpp"

#include <sstream>
#include <fstream>
#include <stdexcept>


struct SvgImage {

    SvgImage(size_t width, size_t height, Rgb background = { 255, 255, 255 })
        : width_(width),
          height_(height)
    {
        draw_rectangle({0, 0, width, height}, background);
    }

    void draw_rectangle(Rect rect, Rgb color) {
        svg_str_ << "<rect ";
        svg_str_ << "x='" << rect.x << "' y='" << rect.y << "' ";
        svg_str_ << "width='" << rect.width << "' height='" << rect.height << "' ";
        svg_str_ << "fill='" << color.to_string() << "' ";
        svg_str_ << "/>\n";
    }

    void save(const std::string& path) const {
        std::ofstream out(path);

        if (!out) {
            throw std::runtime_error("Cannot create file " + path);
        }

        out << "<svg xmlns='http://www.w3.org/2000/svg'\n";
        out << "\twidth='" << width_ << "px' height='" << height_ << "px'\n";
        out << "\tviewBox='0 0 " << width_ << " " << height_ << "'>\n";
        out << svg_str_.str();
        out << "</svg>\n";
    }

    size_t width() const {
        return width_;
    }

    size_t height() const {
        return height_;
    }

private:
    std::stringstream svg_str_;

    size_t width_;
    size_t height_;
};


struct SvgDrawer : ImageDrawer {

    void operator()(const Grid& grid, const ImageConfig& config) override {
        SvgImage image(config.width, config.height, { 255, 255, 255 });

        draw_borders(image, config);
        draw_grid(image, config, grid);

        image.save(config.path);
    }

private:
    void draw_borders(SvgImage& image, const ImageConfig& config) {
        Rgb border_color = { 0, 0, 0 };

        size_t w = config.width;
        size_t h = config.height;
        size_t b = config.border_size;

        image.draw_rectangle({ 0, 0, w, b }, border_color);
        image.draw_rectangle({ 0, 0, b, h }, border_color);
        image.draw_rectangle({ 0, h - b, w, b }, border_color);
        image.draw_rectangle({ w - b, 0, b, h }, border_color);
    }

    void draw_grid(SvgImage& image, const ImageConfig& config, const Grid& grid) {
        Rgb color;
        Rect block_rect = { 0, config.border_size, config.block_size, config.block_size };

        size_t max_occupancy = config.adjust_colors ? grid.max_occupancy() : grid.block_capacity();

        for (size_t row = 0; row < grid.rows(); ++row) {
            block_rect.x = config.border_size;
            for (size_t col = 0; col < grid.cols(); ++col) {
                if (grid.count_at(row, col) == 0) {
                    block_rect.x += config.block_size;
                    continue;
                }
                float density = grid.count_at(row, col)/(float)max_occupancy;
                color = config.color_palette.sample_color(density);

                image.draw_rectangle(block_rect, color);

                block_rect.x += config.block_size;
            }
            block_rect.y += config.block_size;
        }
    }

};
