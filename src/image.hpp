#ifndef image_hpp
#define image_hpp
#include <cstring>
#include <limits>
#include <memory>
#include <stdexcept>
#include <vector>
#include <SOIL.h>

// Own C++ pixel storage; SOIL buffers are released by SOIL immediately after import.
struct Image {
private:
    std::vector<unsigned char> pixels;
    static size_t byteCount(int w, int h, int ch) {
        if (w <= 0 || h <= 0 || ch < 1 || ch > 4) throw std::invalid_argument("invalid image dimensions");
        size_t count = static_cast<size_t>(w);
        if (count > std::numeric_limits<size_t>::max() / static_cast<size_t>(h) / static_cast<size_t>(ch))
            throw std::length_error("image too large");
        return count * static_cast<size_t>(h) * static_cast<size_t>(ch);
    }
public:
    int width = 0, height = 0, channel = 0;
    Image() = default;
    explicit Image(const char * path, int force_channels = 0) { loadFromFile(path, force_channels); }
    bool loadFromFile(const char * path, int force_channels = 0) {
        clear();
        if (force_channels < 0 || force_channels > 4) return false;
        int w = 0, h = 0, ch = 0;
        std::unique_ptr<unsigned char, decltype(&SOIL_free_image_data)> loaded(
            SOIL_load_image(path, &w, &h, &ch, force_channels), &SOIL_free_image_data);
        if (!loaded) return false;
        if (force_channels) ch = force_channels;
        pixels.assign(loaded.get(), loaded.get() + byteCount(w, h, ch));
        width = w; height = h; channel = ch;
        return true;
    }
    unsigned char * data() { return pixels.data(); }
    const unsigned char * data() const { return pixels.data(); }
    operator const unsigned char * () const { return data(); }
    unsigned char & operator()(int w, int h, int ch) { return pixels[channel * (width*h + w) + ch]; }
    const unsigned char & operator()(int w, int h, int ch) const { return pixels[channel * (width*h + w) + ch]; }
    void blankImage(int w, int h, int ch) {
        pixels.assign(byteCount(w, h, ch), 0);
        width = w; height = h; channel = ch;
    }
    bool isNull() const { return pixels.empty(); }
    void clear() {
        std::vector<unsigned char>().swap(pixels);
        width = height = channel = 0;
    }
    bool copy(const Image & other, int w, int h) {
        if (isNull() || other.isNull() || channel != other.channel) return false;
        if (w < 0 || h < 0 || other.width > width - w || other.height > height - h) return false;
        for (int row = 0; row < other.height; ++row)
            std::memcpy(&(*this)(w,h+row,0), &other(0,row,0), static_cast<size_t>(other.width)*other.channel);
        return true;
    }
    bool save(const char * path) const {
        return SOIL_save_image(path, SOIL_SAVE_TYPE_BMP, width, height, channel, data()) != 0;
    }
};
#endif
