#ifndef image_hpp
#define image_hpp
#include <cstring>
#include <SOIL.h>

// 简单包装了 SOIL 库。
// 用于 load, save 图片，和将图片2拷贝到图片1的指定位置，用于生成 texture atlas。
struct Image {
    unsigned char * data;
    int width, height, channel;
    Image() {
        data = 0;
    }
    Image(unsigned char * data, int width, int height, int channel) {
        this->data = data;
        this->width = width;
        this->height = height;
        this->channel = channel;
    }
    Image(const char * path, int force_channels = 0) {
        loadFromFile(path, force_channels);
    }
    ~Image() {
        if (data) {
//            SOIL_free_image_data(data);
            delete [] data;
        }
    }
    
    // 如果设置了 force_channels，图片会强制以 force_channels 个通道数打开。
    // 然而此时 channels 仍为图片原始的通道数，所以如果以 force_channels 模式打开要设一遍 channels
    // 任何图片都应以 4 通道打开（alpha 透明通道）
    bool loadFromFile(const char * path, int force_channels = 0) {
        data = SOIL_load_image(path, &width, &height, &channel, force_channels);
        if (force_channels) channel = force_channels;
        return data;
    }
    operator unsigned char * () {
        return data;
    }
    unsigned char & operator () (int w, int h, int ch) const {
        return data[channel * (width * h + w) + ch];
    }
    
    // 新建一张透明图片
    void blankImage(int w, int h, int ch) {
        this->width = w;
        this->height = h;
        this->channel = ch;
        if (data) delete [] data;
        data = new unsigned char[w * h * ch]();
    }
    
    bool isNull() {
        return data == 0;
    }
    
    // 清空
    void clear() {
        if (data) delete [] data;
        this->data = 0;
        this->width = 0;
        this->height = 0;
        this->channel = 0;
    }
    
    // 将图片2拷贝到图片1的指定位置，用于生成 texture atlas。
    bool copy(const Image & other, int w, int h) {
        // 两张图片通道应相等
        if (channel != other.channel) return false;
        if (w < 0 || w + other.width > width || h < 0 || h + other.height > height) return false;
        for (int r = 0; r < other.height; ++r) {
            memcpy(&(*this)(w, h + r, 0), &other(0, r, 0), other.width * other.channel);
        }
        return true;
    }
    // 由于 SOIL 库的特性，目前只能正常保存 bmp 格式。。
    bool save(const char * path) {
        return SOIL_save_image(path, SOIL_SAVE_TYPE_BMP, width, height, channel, data);
    }
};

#endif /* image_hpp */
