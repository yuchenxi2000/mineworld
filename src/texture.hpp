#ifndef texture_hpp
#define texture_hpp

#include <iostream>
#include "config.hpp"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "image.hpp"
#include <unordered_map>

namespace mineworld {
    /*
     * the location of a texture in texture atlas
     */
    struct texture_loc_t {
        int x, y;
        GLuint TexID;
        texture_loc_t() : x(0), y(0), TexID(0) {}
        texture_loc_t(int x, int y, GLuint id) : x(x), y(y), TexID(id) {}
    };
    
    /*
     * create a single texture
     */
    GLuint create2DTexture(const std::string & path);
    
    /*
     * create and manage texture atlas
     * call loadTexture to add a texture to atlas
     * call finishLoadTexture to pass texture to GPU
     * (must be called after texture loading finished)
     */
    class TextureManager {
    private:
        int CAPCITY_WIDTH = 0, CAPCITY_HEIGHT = 0;
        Image texture_image;
        int next_w = 0, next_h = 0;
        GLuint textureID = 0;
        std::unordered_map<std::string, texture_loc_t> locations;
    public:
        int MAX_TEXTURE_SIZE = 0;
        TextureManager() = default;
        TextureManager(const TextureManager &) = delete;
        TextureManager & operator=(const TextureManager &) = delete;
        void init();
        ~TextureManager() {
            release();
        }
        void release() {
            if (textureID) glDeleteTextures(1, &textureID);
            textureID = 0;
            texture_image.clear();
            locations.clear();
        }
        
        GLuint getTextureID() {
            return textureID;
        }
        texture_loc_t loadTexture(const std::string & imagePath);
        void finishLoadTexture();
        
    };
    
    extern TextureManager gtexturemanager;
    
}

#endif /* texture_hpp */
