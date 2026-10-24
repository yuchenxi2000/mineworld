#ifndef util_hpp
#define util_hpp
#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <cmath>
#include <glm/glm.hpp>

namespace mineworld {
    
    struct hash_glm_ivec3 { // hash function for ivec3
        size_t operator () (const glm::ivec3 & v) const {
            size_t hash = 0;
            for (int axis = 0; axis < 3; ++axis)
                hash ^= std::hash<int>{}(v[axis]) + size_t(0x9e3779b9) + (hash << 6) + (hash >> 2);
            return hash;
        }
    };
    
    struct block_loc_t {
        glm::ivec3 chunkpos;
        glm::ivec3 offset;
        block_loc_t() : chunkpos(0), offset(0) {}
        block_loc_t(const glm::ivec3 & chunkpos, const glm::ivec3 & offset) {
            this->chunkpos = chunkpos;
            this->offset = offset;
        }
        block_loc_t(const glm::ivec3 & pos) {
            offset = {
                pos.x & 0xf,
                pos.y & 0xf,
                pos.z & 0xf
            };
            chunkpos = (pos - offset);
        }
        block_loc_t & operator += (const glm::ivec3 & iv) {
            glm::ivec3 loffset = offset + iv;
            offset = {
                loffset.x & 0xf,
                loffset.y & 0xf,
                loffset.z & 0xf
            };
            chunkpos += (loffset - offset);
            return *this;
        }
        block_loc_t & operator -= (const glm::ivec3 & iv) {
            glm::ivec3 loffset = offset - iv;
            offset = {
                loffset.x & 0xf,
                loffset.y & 0xf,
                loffset.z & 0xf
            };
            chunkpos += (loffset - offset);
            return *this;
        }
    };
    
    struct entity_pos_t {
        glm::ivec3 chunkpos;
        glm::vec3 offset;
        entity_pos_t() : chunkpos(0), offset(0) {}
        entity_pos_t(const glm::ivec3 & chunkpos, const glm::vec3 & offset) {
            this->chunkpos = chunkpos;
            this->offset = offset;
        }
        entity_pos_t & operator += (const glm::vec3 & v) {
            offset += v;
            const glm::ivec3 cells(glm::floor(offset / 16.0f));
            chunkpos += cells * 16;
            offset -= glm::vec3(cells * 16);
            return *this;
        }
        entity_pos_t & operator -= (const glm::vec3 & v) { return *this += -v; }
        operator block_loc_t () const {
            block_loc_t lblockloc;
            lblockloc.chunkpos = chunkpos;
            lblockloc.offset = offset;
            return lblockloc;
        }
    };
    
    std::string readFromFile(std::string path);
    
    GLuint compile(const char * vShaderCode, const char * fShaderCode);
    
}

#endif /* util_hpp */
