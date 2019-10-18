#include "texture.hpp"
namespace mineworld {
    
    Image createMissingTexture() {
        Image image;
        image.width = config.TEXTURE_SIZE;
        image.height = config.TEXTURE_SIZE;
        image.channel = 4;
        int len = image.width * image.height * image.channel;
        image.data = new unsigned char[len];
        memset(image.data, 0, len);
        
        for (int i = 0; i < config.TEXTURE_SIZE / 2; ++i) {
            for (int j = 0; j < config.TEXTURE_SIZE / 2; ++j) {
                image(i, j, 0) = 255;
                image(i, j, 1) = 231;
                image(i, j, 2) = 42;
                image(i, j, 3) = 255;
            }
        }
        for (int i = config.TEXTURE_SIZE / 2; i < config.TEXTURE_SIZE; ++i) {
            for (int j = config.TEXTURE_SIZE / 2; j < config.TEXTURE_SIZE; ++j) {
                image(i, j, 0) = 255;
                image(i, j, 1) = 231;
                image(i, j, 2) = 42;
                image(i, j, 3) = 255;
            }
        }
        for (int i = 0; i < config.TEXTURE_SIZE / 2; ++i) {
            for (int j = config.TEXTURE_SIZE / 2; j < config.TEXTURE_SIZE; ++j) {
                image(i, j, 0) = 0;
                image(i, j, 1) = 0;
                image(i, j, 2) = 0;
                image(i, j, 3) = 255;
            }
        }
        for (int i = config.TEXTURE_SIZE / 2; i < config.TEXTURE_SIZE; ++i) {
            for (int j = 0; j < config.TEXTURE_SIZE / 2; ++j) {
                image(i, j, 0) = 0;
                image(i, j, 1) = 0;
                image(i, j, 2) = 0;
                image(i, j, 3) = 255;
            }
        }
        return image;
    }
    
    GLuint create2DTexture(const std::string & path) {
        Image img(path.c_str(), 4);
        if (img.isNull()) {
            std::cerr << "[texture] failed to load texture from " << path << std::endl;
        }
        GLuint textureID;
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexImage2D(
                     GL_TEXTURE_2D,
                     0, GL_RGBA, img.width, img.height, 0, GL_RGBA,
                     GL_UNSIGNED_BYTE, img
                     );
        return textureID;
    }
    
    void TextureManager::init() {
        next_h = 0;
        next_w = 0;
        
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);
        
        glGetIntegerv(GL_MAX_TEXTURE_SIZE, &MAX_TEXTURE_SIZE);
        std::cout << "[texture] max atlas size " << MAX_TEXTURE_SIZE << std::endl;
        
        if (MAX_TEXTURE_SIZE > config.ATLAS_SIZE_UPPER_BOUND) MAX_TEXTURE_SIZE = config.ATLAS_SIZE_UPPER_BOUND;
        
        if (config.TEXTURE_SIZE > MAX_TEXTURE_SIZE) {
            std::cerr << "[texture] error : texture size larger than atlas size. terminate" << std::endl;
            exit(EXIT_FAILURE);
        }
        
        std::cout << "[texture] current atlas size " << MAX_TEXTURE_SIZE << std::endl;
        
        CAPCITY_WIDTH = MAX_TEXTURE_SIZE / config.TEXTURE_SIZE;
        CAPCITY_HEIGHT = MAX_TEXTURE_SIZE / config.TEXTURE_SIZE;
        
        // load missing texture
        texture_image.blankImage(MAX_TEXTURE_SIZE, MAX_TEXTURE_SIZE, 4);
        Image missingtex = createMissingTexture();
        
        texture_image.copy(missingtex, 0, 0);
        
        ++next_w;
        if (next_w >= CAPCITY_WIDTH) {
            next_w = 0;
            ++next_h;
        }
        if (next_h >= CAPCITY_HEIGHT) {
            std::cerr << "[texture] atlas size is too small. texture not loaded: missing texture" << std::endl;
        }
        
    }
    
    texture_loc_t TextureManager::loadTexture(const std::string & imagePath) {
        Image image(imagePath.c_str(), 4);
        if (image.isNull()) {
            std::cerr << "[texture] failed to load image from " << imagePath << std::endl;
            return texture_loc_t(0, 0, 0);
        }else if (image.width != config.TEXTURE_SIZE || image.height != config.TEXTURE_SIZE) {
            std::cerr << "[texture] image size should equal to config.TEXTURE_SIZE: " << imagePath << std::endl;
            return texture_loc_t(0, 0, 0);
        }else{
            int h = config.TEXTURE_SIZE * next_h;
            int w = config.TEXTURE_SIZE * next_w;
            
            texture_image.copy(image, w, h);
            
            ++next_w;
            if (next_w >= CAPCITY_WIDTH) {
                next_w = 0;
                ++next_h;
            }
            if (next_h >= CAPCITY_HEIGHT) {
                std::cerr << "[texture] atlas size is too small. texture not loaded: " << imagePath << std::endl;
                return texture_loc_t(0, 0, 0);
            }
            return texture_loc_t(w, h, 0);
        }
    }
    
    
    void TextureManager::finishLoadTexture() {
        if (!texture_image.isNull()) {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, textureID);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, MAX_TEXTURE_SIZE, MAX_TEXTURE_SIZE, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture_image);
            
            texture_image.clear();
        }
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    
    TextureManager gtexturemanager;
}
