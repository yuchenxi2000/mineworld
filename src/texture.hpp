#ifndef texture_hpp
#define texture_hpp

#include <iostream>
#include "config.hpp"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "image.hpp"

namespace mineworld {
    /*
     * the location of a texture in texture atlas
     */
    struct texture_loc_t {
        int x, y;
        GLuint TexID;
        texture_loc_t() {}
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
        int CAPCITY_WIDTH, CAPCITY_HEIGHT;
        Image texture_image;
        int next_w, next_h;
        GLuint textureID;
    public:
        int MAX_TEXTURE_SIZE;
        void init();
        ~TextureManager() {
            glDeleteTextures(1, &textureID);
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
