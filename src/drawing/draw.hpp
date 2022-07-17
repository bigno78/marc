#pragma once

#include "../grid.hpp"
#include "../types.hpp"

#include <string>
#include <iostream>
#include <stdexcept>


struct Rgb {
    uint8_t red;
    uint8_t green;
    uint8_t blue;

    std::string to_string() {
        return "rgb(" + std::to_string(red) + ", "
                    + std::to_string(green) + ", "
                    + std::to_string(blue) + ")";
    }

};

struct ColorPalette {

    ColorPalette(const std::vector<Rgb>& colors) : colors(colors) {}

    static ColorPalette blue() {
        return ColorPalette({
                //{247,251,255}, 
                //{222,235,247},
                //{198,219,239},
                //{158,202,225}, 
                {107,174,214},
                {66,146,198},
                {33,113,181},
                {8,81,156},
                {8,48,107}
            });
    }

    Rgb sample_color(float val) const {
        if (val == 0) {
            return { 255, 255, 255 };
        }

        float w = 1.0f/(colors.size() - 1);

        size_t i = (size_t) (val*(colors.size() - 1));

        Rgb a = colors[i];
        Rgb b =  colors[i+1];

        float t = (val - i*w)/w;

        return { interpolate(a.red, b.red, t),
                interpolate(a.green, b.green, t),
                interpolate(a.blue, b.blue, t) };
    }

private:
    uint8_t interpolate(uint8_t a, uint8_t b, float t) const {
        return (uint8_t) ((1 - t)*a + t*b);
    }

private:
    std::vector<Rgb> colors;
};


struct ImageConfig {
    std::string path;

    size_t width;
    size_t height;

    size_t block_size = 1;

    size_t border_size = 2;

    ColorPalette color_palette = ColorPalette::blue();

    size_t viewport_width = 600;
    size_t viewport_height = 600;

    bool adjust_colors = false;
};


struct ImageDrawer {

    virtual void operator()(const Grid& grid, const ImageConfig& config) = 0;

    virtual ~ImageDrawer() = default;

};
