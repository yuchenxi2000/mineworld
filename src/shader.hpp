#ifndef shader_hpp
#define shader_hpp

#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cstring>

namespace mineworld {
    class Cell;
    
    class Shader {
        /*
         * attributes,
         * program id, texture id,
         * uniform location, etc.
         */
        struct {
            GLuint program;
            GLuint textureID, skyboxID;
            GLint blocktextureloc, viewloc, projectionloc, posoffsetloc;
            GLint skyboxtextureloc;
        } Attrib{};
    public:
        Shader() = default;
        Shader(const Shader &) = delete;
        Shader & operator=(const Shader &) = delete;
        ~Shader() { release(); }
        void release();
        void init();
        /*
         * use the shader when render scene
         */
        void use() {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, Attrib.textureID);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_CUBE_MAP, Attrib.skyboxID);
            glUseProgram(Attrib.program);
            glUniform1i(Attrib.blocktextureloc, 0);
            glUniform1i(Attrib.skyboxtextureloc, 1);
        }
        /*
         * pass view, projection matrix to program
         */
        void setuniform(const glm::mat4 & viewmat, const glm::mat4 & projectionmat) {
            glUniformMatrix4fv(Attrib.viewloc, 1, GL_FALSE, glm::value_ptr(viewmat));
            glUniformMatrix4fv(Attrib.projectionloc, 1, GL_FALSE, glm::value_ptr(projectionmat));
        }
        void drawCell(Cell * cell);
    };
    
    class SkyBoxShader {
        /*
         * attributes,
         * program id, texture id,
         * uniform location, etc.
         */
        struct {
            GLuint program;
            GLint projectionloc, skyboxviewloc, skyboxloc;
            GLuint skyboxID;
            GLuint vao, vbo;
        } Attrib{};
        void loadSkyBox();
        static const float cube[108];
    public:
        SkyBoxShader() = default;
        SkyBoxShader(const SkyBoxShader &) = delete;
        SkyBoxShader & operator=(const SkyBoxShader &) = delete;
        ~SkyBoxShader() { release(); }
        void release();
        void init();
        /*
         * use the shader when render scene
         */
        void use() {
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_CUBE_MAP, Attrib.skyboxID);
            glUseProgram(Attrib.program);
            glUniform1i(Attrib.skyboxloc, 1);
        }
        /*
         * pass view, projection matrix to program
         */
        void setuniform(const glm::mat4 & skyboxviewmat, const glm::mat4 & projectionmat) {
            glUniformMatrix4fv(Attrib.skyboxviewloc, 1, GL_FALSE, glm::value_ptr(skyboxviewmat));
            glUniformMatrix4fv(Attrib.projectionloc, 1, GL_FALSE, glm::value_ptr(projectionmat));
        }
        GLuint getTextureID() {
            return Attrib.skyboxID;
        }
        void drawSkyBox() {
            glBindVertexArray(Attrib.vao);
            glDrawArrays(GL_TRIANGLES, 0, 36);
            glBindVertexArray(0);
        }
    };
    
    class CrosshairShader {
        struct {
            GLuint program, vao, vbo;
            GLuint textureID;
            GLint textureloc;
        } Attrib{};
        
        // vertices
        float cross[30] = {
            -1, 1, 0, 0, 1,
            -1, -1, 0, 0, 0,
            1, -1, 0, 1, 0,
            1, -1, 0, 1, 0,
            1, 1, 0, 1, 1,
            -1, 1, 0, 0, 1,
        };
        float ratio = 0.05f; // crosshair height divided by window height
    public:
        CrosshairShader() = default;
        CrosshairShader(const CrosshairShader &) = delete;
        CrosshairShader & operator=(const CrosshairShader &) = delete;
        ~CrosshairShader() { release(); }
        void release();
        void init();
        void drawCrosshair() {
            glUseProgram(Attrib.program);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, Attrib.textureID);
            glUniform1i(Attrib.textureloc, 0);
            glEnable(GL_COLOR_LOGIC_OP);
            glLogicOp(GL_INVERT); // inverse of background color
            glBindVertexArray(Attrib.vao);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            glBindVertexArray(0);
            glDisable(GL_COLOR_LOGIC_OP);
        }
        
        // resize when window size changed by user.
        // otherwise, height may not equal to width
        void resize(int width, int height) {
            if (width <= 0 || height <= 0 || !Attrib.vbo) return;
            float tmp[30];
            memcpy(tmp, cross, sizeof(cross));
            for (int i = 0; i < 6; ++i) {
                tmp[5 * i] *= ratio * height / width;
                tmp[5 * i + 1] *= ratio;
            }
            glBindBuffer(GL_ARRAY_BUFFER, Attrib.vbo);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(cross), tmp);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }
    };
    
    extern Shader gblockshader;
    extern SkyBoxShader gskyboxshader;
    extern CrosshairShader gcrosshairshader;
}

#endif /* shader_hpp */
