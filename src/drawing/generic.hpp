#pragma once

#include <drawing/draw.hpp>

template<typename Image>
struct GenericDrawer : ImageDrawer {

    void operator()(const Grid& grid, const ImageConfig& config) override {
        Image image(config.width, config.height, { 255, 255, 255 });

        draw_borders(image, config);
        draw_grid(image, config, grid);

        image.save(config.path);
    }

private:
    void draw_borders(Image& image, const ImageConfig& config) {
        Rgb border_color = { 0, 0, 0 };

        size_t w = config.width;
        size_t h = config.height;
        size_t b = config.border_size;

        image.draw_rectangle({ 0, 0, w, b }, border_color);
        image.draw_rectangle({ 0, 0, b, h }, border_color);
        image.draw_rectangle({ 0, h - b, w, b }, border_color);
        image.draw_rectangle({ w - b, 0, b, h }, border_color);
    }

    void draw_grid(Image& image, const ImageConfig& config, const Grid& grid) {
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
