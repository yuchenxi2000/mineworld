#ifndef image_hpp
#define image_hpp
#include <cstring>
#include <SOIL.h>

// simple wrapper for SOIL
// load, save image,
// copy contents of one image to another image (creating texture atlas)
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
    
    // If force_channels is specified, image would be open with channels of force_channels.
    // While value of channels is still channels of the origin image,
    // so I set the channels variable manually.
    // Any image should be open with 4 channels (with alpha channel)
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
    
    // get a new blank image
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
    
    // delete image buffer
    void clear() {
        if (data) delete [] data;
        this->data = 0;
        this->width = 0;
        this->height = 0;
        this->channel = 0;
    }
    
    // copy contents of one image to another image
    bool copy(const Image & other, int w, int h) {
        // 两张图片通道应相等
        if (channel != other.channel) return false;
        if (w < 0 || w + other.width > width || h < 0 || h + other.height > height) return false;
        for (int r = 0; r < other.height; ++r) {
            memcpy(&(*this)(w, h + r, 0), &other(0, r, 0), other.width * other.channel);
        }
        return true;
    }
    
    // Unforturnately, image can only be saved with bmp format.
    // Maybe one of SOIL's features...
    bool save(const char * path) {
        return SOIL_save_image(path, SOIL_SAVE_TYPE_BMP, width, height, channel, data);
    }
};

#endif /* image_hpp */
