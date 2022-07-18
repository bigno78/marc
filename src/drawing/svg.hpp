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
