#pragma once

#include "draw.hpp"

#include <sstream>
#include <fstream>
#include <stdexcept>


// x and y are the coordinates of upper left corner
struct Rect {
    size_t x;
    size_t y;
    size_t width;
    size_t height;
};

struct Style {
    std::string fill_color;
    float fill_opacity;
    std::string border_color;
    float border_width;
};


struct SvgImage {

    SvgImage(float width, float height) : max_x(width), max_y(height) {}

    void draw_rectangle(Rect rect, const Style& style) {
        svg_str << "<rect ";
        
        svg_str << "x='" << rect.x << "' y='" << rect.y << "' ";
        svg_str << "width='" << rect.width << "' height='" << rect.height << "' ";
        svg_str << "fill='" << style.fill_color << "' ";
        
        if (style.fill_opacity < 1) {
            svg_str << "fill-opacity='" << style.fill_opacity << "' ";
        }
        
        if (style.border_width != 0) {
            svg_str << "stroke='" << style.border_color << "' ";
            svg_str << "stroke-width='" << style.border_width << "' ";
        }

        svg_str << "/>\n";
    }

    void save(const std::string& path, size_t width_in_pixels, size_t height_in_pixels) const {
        std::ofstream out(path);

        if (!out) {
            throw std::runtime_error("Cannot create file " + path);
        }

        out << "<svg xmlns='http://www.w3.org/2000/svg'\n";
        out << "\twidth='" << width_in_pixels << "px' height='" << height_in_pixels << "px'\n";
        out << "\tviewBox='0 0 " << max_x << " " << max_y << "'>\n";
        out << svg_str.str();
        out << "</svg>\n";
    }

private:

    std::stringstream svg_str;

    float max_x;
    float max_y;
};


struct SvgDrawer : ImageDrawer {

    void operator()(const Grid& grid, const ImageConfig& config) override {
        SvgImage image(config.width, config.height);

        draw_background(image, config);
        draw_borders(image, config);
        draw_grid(image, config, grid);

        image.save(config.path, config.width, config.height);
    }

private:

    void draw_background(SvgImage& image, const ImageConfig& config) {
        Style style { "white", 1, "black", 0 };
        image.draw_rectangle({0, 0, config.width, config.height}, style);
    }

    void draw_borders(SvgImage& image, const ImageConfig& config) {
        Style border_style { "black", 1, "black", 0 };

        size_t w = config.width;
        size_t h = config.height;
        size_t b = config.border_size;

        image.draw_rectangle({ 0, 0, w, b }, border_style);
        image.draw_rectangle({ 0, 0, b, h }, border_style);
        image.draw_rectangle({ 0, h - b, w, b }, border_style);
        image.draw_rectangle({ w - b, 0, b, h }, border_style);
    }

    void draw_grid(SvgImage& image, const ImageConfig& config, const Grid& grid) {
        Style style { "black", 1, "black", 0 };
        Rect block_rect = { 0, config.border_size, config.block_size, config.block_size };

        float max_occupancy = config.adjust_colors ? grid.max_occupancy() : grid.block_capacity();

        for (size_t row = 0; row < grid.rows(); ++row) {
            block_rect.x = config.border_size;
            for (size_t col = 0; col < grid.cols(); ++col) {
                if (grid.count_at(row, col) == 0) {
                    block_rect.x += config.block_size;
                    continue;
                }
                float density = grid.count_at(row, col)/max_occupancy;
                style.fill_color = config.color_palette.sample_color(density).to_string();
                
                image.draw_rectangle(block_rect, style);
                
                block_rect.x += config.block_size;
            }
            block_rect.y += config.block_size;
        }
    }

};
