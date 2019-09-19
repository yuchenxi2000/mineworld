#include "texture.hpp"
namespace mineworld2 {
    
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
    
    void mineworld2::TextureManager::getGLInfo() {
        glGetIntegerv(GL_MAX_TEXTURE_SIZE, &MAX_TEXTURE_SIZE);
        std::cout << "[texture] max atlas size " << MAX_TEXTURE_SIZE << std::endl;
        
        if (MAX_TEXTURE_SIZE > config.ATLAS_SIZE_UPPER_BOUND) MAX_TEXTURE_SIZE = config.ATLAS_SIZE_UPPER_BOUND;
        
        if (config.TEXTURE_SIZE > MAX_TEXTURE_SIZE) {
            std::cerr << "[texture] error : texture size larger than atlas size. terminate" << std::endl;
            exit(-1);
        }

        std::cout << "[texture] current atlas size " << MAX_TEXTURE_SIZE << std::endl;
        
        CAPCITY_WIDTH = MAX_TEXTURE_SIZE / config.TEXTURE_SIZE;
        CAPCITY_HEIGHT = MAX_TEXTURE_SIZE / config.TEXTURE_SIZE;
    }
    
    void mineworld2::TextureManager::atlas_to_GPU() {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureID[TexCnt]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, MAX_TEXTURE_SIZE, MAX_TEXTURE_SIZE, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture_image);
    }
    
    
    mineworld2::Textureloc mineworld2::TextureManager::loadTexture(const std::string & imagePath) {
        Image image(imagePath.c_str(), 4);
        if (image.isNull()) {
            std::cerr << "[texture] failed to load image" << std::endl;
            return Textureloc(-1, -1, -1);
        }else if (image.width != config.TEXTURE_SIZE || image.height != config.TEXTURE_SIZE) {
            std::cerr << "[texture] image size should equal to config.TEXTURE_SIZE" << std::endl;
            return Textureloc(-1, -1, -1);
        }else if (TexCnt >= MAX_TEXTURE_NUM){
            return Textureloc(-1, -1, -1);
        }else{
            
            if (next_h >= CAPCITY_HEIGHT || next_w >= CAPCITY_WIDTH) {
                atlas_to_GPU();
                
                ++TexCnt;
                if (TexCnt >= MAX_TEXTURE_NUM)
                    return Textureloc(-1, -1, -1);
                
                next_h = 0; next_w = 0;
            }
            
            int h = config.TEXTURE_SIZE * next_h;
            int w = config.TEXTURE_SIZE * next_w;
            
            texture_image.copy(image, w, h);
            
            ++next_w;
            if (next_w >= CAPCITY_WIDTH) {
                next_w = 0;
                ++next_h;
            }
            return Textureloc(w, h, TexCnt);
        }
    }
    
    
    void mineworld2::TextureManager::finishLoadTexture() {
        if (!texture_image.isNull()) {
            atlas_to_GPU();
            ++TexCnt;
            texture_image.clear();
        }
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    
    mineworld2::TextureManager gtexturemanager;
}
