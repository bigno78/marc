#include <drawing/draw.hpp>
#include <stb_image_write.h>

#include <cstring> // memcpy

struct StbImage {

    StbImage(size_t width, size_t height, Rgb background = { 255, 255, 255 })
        : width_(width),
          height_(height),
          data_(width*height, rgb_to_u32(background)) { }

    void draw_rectangle(Rect rect, Rgb color) {
        for (size_t x = rect.x; x < rect.x + rect.width; ++x) {
            for (size_t y = rect.y; y < rect.y + rect.height; ++y) {
                write_pixel(x, y, color);
            }
        }
    }

    void save(const std::string& path) const {
        int res = stbi_write_png(path.c_str(), (int)width_, (int)height_, 4, data_.data(), (int)width_*4);

        if (!res) {
            throw std::runtime_error("stb_image: Cannot save image into file " + path + ".");
        }
    }

    size_t width() const {
        return width_;
    }

    size_t height() const {
        return height_;
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
};
