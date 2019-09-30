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
        static const int MAX_TEXTURE_NUM = 16;
        
        int CAPCITY_WIDTH, CAPCITY_HEIGHT;
        Image texture_image;
        int next_w = 0, next_h = 0;
        
        void getGLInfo();
        void atlas_to_GPU();

    public:
        // GL constant
        int MAX_TEXTURE_SIZE;
        
        int TexCnt = 0;
        GLuint textureID[MAX_TEXTURE_NUM];
        
        GLuint getTextureID() {
            return textureID[0];
        }
        
        TextureManager() {
            
        }
        void init() {
            glGenTextures(1, &textureID[0]);
            glBindTexture(GL_TEXTURE_2D, textureID[0]);
            getGLInfo();
            texture_image.blankImage(MAX_TEXTURE_SIZE, MAX_TEXTURE_SIZE, 4);
        }
        ~TextureManager() {
            glDeleteTextures(TexCnt, textureID);
        }
        
        texture_loc_t loadTexture(const std::string & imagePath);
        void finishLoadTexture();
        
    };
    
    extern TextureManager gtexturemanager;
    
}

#endif /* texture_hpp */
