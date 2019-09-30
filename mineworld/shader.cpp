#include "shader.hpp"
#include "config.hpp"
#include "texture.hpp"
#include "util.hpp"
namespace mineworld {
    
    static glm::vec3 delta[8] = {
        {0, 0, 0},
        {CELL_X, 0, 0},
        {0, CELL_Y, 0},
        {0, 0, CELL_Z},
        {0, CELL_Y, CELL_Z},
        {CELL_X, 0, CELL_Z},
        {CELL_X, CELL_Y, 0},
        {CELL_X, CELL_Y, CELL_Z},
    };
    
    void Shader::drawCell(Cell * cell) {
        const entity_pos_t & playerpos = handler.player->entitypos;
        glm::vec3 positionoffset = glm::vec3(playerpos.chunkpos - cell->posoffset) + playerpos.offset;
        
        for (int i = 0; i < 8; ++i) {
            auto distance = glm::dot(handler.player->lookdirection, delta[i] - positionoffset);
            // draw visible chunks
            // Negative distance <=> chunk is behind.
            if (distance > 0 && distance < config.VISIBLE_DISTANCE * CELL_X) {
                glUniform3fv(Attrib.posoffsetloc, 1, glm::value_ptr(positionoffset));
                cell->glbuffer.draw();
                return;
            }
        }
    }
    
    const float SkyBoxShader::cube[108] = {
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        
        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,
        
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        
        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,
        
        -1.0f,  1.0f, -1.0f,
        1.0f,  1.0f, -1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,
        
        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
        1.0f, -1.0f,  1.0f
    };
    
    void SkyBoxShader::loadSkyBox() {
        glGenTextures(1, &Attrib.skyboxID);
        glBindTexture(GL_TEXTURE_CUBE_MAP, Attrib.skyboxID);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        for (int i = 0; i < 6; i++) {
            Image image(config.skybox.faces[i].c_str(), 3);
            if (image.isNull()) {
                std::cout << "[skybox] cannot open " << config.skybox.faces[i] << std::endl;
                exit(-1);
            }
            glTexImage2D(
                         GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                         0, GL_RGB, image.width, image.height, 0, GL_RGB, GL_UNSIGNED_BYTE, image
                         );
        }
    }
    
    void Shader::init() {
        std::string fsh = readFromFile(config.blockshader.fsh);
        std::string vsh = readFromFile(config.blockshader.vsh);
        Attrib.program = compile(vsh.c_str(), fsh.c_str());
        Attrib.textureID = gtexturemanager.getTextureID();
        Attrib.skyboxID = gskyboxshader.getTextureID();
        Attrib.blocktextureloc = glGetUniformLocation(Attrib.program, "BlockTexture");
        Attrib.viewloc = glGetUniformLocation(Attrib.program, "view");
        Attrib.projectionloc = glGetUniformLocation(Attrib.program, "projection");
        Attrib.skyboxtextureloc = glGetUniformLocation(Attrib.program, "skyboxtexture");
        Attrib.posoffsetloc = glGetUniformLocation(Attrib.program, "positionoffset");
    }
    
    void SkyBoxShader::init() {
        std::string fsh = readFromFile(config.skybox.skyboxfsh);
        std::string vsh = readFromFile(config.skybox.skyboxvsh);
        Attrib.program = compile(vsh.c_str(), fsh.c_str());
        Attrib.skyboxloc = glGetUniformLocation(Attrib.program, "skybox");
        Attrib.skyboxviewloc = glGetUniformLocation(Attrib.program, "skyboxview");
        Attrib.projectionloc = glGetUniformLocation(Attrib.program, "projection");
        loadSkyBox();
        glGenVertexArrays(1, &Attrib.vao);
        glGenBuffers(1, &Attrib.vbo);
        glBindVertexArray(Attrib.vao);
        glBindBuffer(GL_ARRAY_BUFFER, Attrib.vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(cube), cube, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
        glEnableVertexAttribArray(0);
        glBindVertexArray(0);
    }
    
    void CrosshairShader::init() {
        std::string fsh = readFromFile(config.crosshair.crossfsh);
        std::string vsh = readFromFile(config.crosshair.crossvsh);
        Attrib.program = compile(vsh.c_str(), fsh.c_str());
        Attrib.textureID = create2DTexture(config.crosshair.texturePath);
        Attrib.textureloc = glGetUniformLocation(Attrib.program, "crosstex");
        glGenVertexArrays(1, &Attrib.vao);
        glGenBuffers(1, &Attrib.vbo);
        glBindVertexArray(Attrib.vao);
        glBindBuffer(GL_ARRAY_BUFFER, Attrib.vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(cross), cross, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(1);
        glBindVertexArray(0);
        
        resize(setting.WINDOW_WIDTH, setting.WINDOW_HEIGHT);
    }
    
    Shader gblockshader;
    SkyBoxShader gskyboxshader;
    CrosshairShader gcrosshairshader;
}
