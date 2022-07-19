#pragma once

#include <drawing/draw.hpp>
#include <stb_image_write.h>

#include <cstring> // memcpy
#include <map>

int write_png(char const *filename, int w, int h, int comp, const void *data) {
    return stbi_write_png(filename, w, h, 4, data, w*4);
}

int write_jpg(char const *filename, int w, int h, int comp, const void *data) {
    // quality is 1 - 100, use a compromise of 50
    return stbi_write_jpg(filename, w, h, 4, data, 50);
}

struct StbImage {

    StbImage(size_t width, size_t height, Rgb background = { 255, 255, 255 })
        : width_(width),
          height_(height),
          data_(width*height, rgb_to_u32(background)) { }

    size_t width() const {
        return width_;
    }

    size_t height() const {
        return height_;
    }

    void draw_rectangle(Rect rect, Rgb color) {
        for (size_t x = rect.x; x < rect.x + rect.width; ++x) {
            for (size_t y = rect.y; y < rect.y + rect.height; ++y) {
                write_pixel(x, y, color);
            }
        }
    }

    void save(const std::string& path, ImageFormat format) const {
        auto it = write_functions_.find(format);

        if (it == write_functions_.end()) {
            throw std::runtime_error("StbImage: Unsupported format.");
        }

        auto write_func = it->second;
        int result = write_func(path.c_str(), (int)width_, (int)height_, 4, data_.data());

        if (!result) {
            throw std::runtime_error("StbImage: Cannot save image into file " + path + ".");
        }
    }


private:
    void write_pixel(size_t x, size_t y, Rgb color) {
        data_[y*width_ + x] = rgb_to_u32(color);
    }

    uint32_t rgb_to_u32(Rgb color) {
        uint32_t packed_color = 0;
        uint8_t bytes[4] = { color.red, color.green, color.blue, 255u };
        std::memcpy(&packed_color, bytes, 4);
        return packed_color;
    }

    size_t width_;
    size_t height_;

    std::vector<uint32_t> data_;

    std::map<ImageFormat, int(*)(const char*, int, int, int, const void*)> write_functions_ = {
        { ImageFormat::png, write_png },
        { ImageFormat::jpg, write_jpg },
        { ImageFormat::bmp, stbi_write_bmp },
        { ImageFormat::tga, stbi_write_tga }
    };
};
