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
            size_t x = v.x;
            size_t y = v.y;
#ifdef __amd64__
            x = (x << 32) + v.z;
            x ^= y << 16;
#else
            x = x + ~v.z;
            x ^= y << 8;
#endif
            return std::hash<size_t> () (x);
        }
    };
    
    struct block_loc_t {
        glm::ivec3 chunkpos;
        glm::ivec3 offset;
        block_loc_t() {}
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
        entity_pos_t() {}
        entity_pos_t(const glm::ivec3 & chunkpos, const glm::vec3 & offset) {
            this->chunkpos = chunkpos;
            this->offset = offset;
        }
        entity_pos_t & operator += (const glm::vec3 & v) {
            offset += v;
            glm::vec3 loffset = {
                16.0 * floorf(offset.x / 16.0),
                16.0 * floorf(offset.y / 16.0),
                16.0 * floorf(offset.z / 16.0)
            };
            chunkpos += loffset;
            offset -= loffset;
            return *this;
        }
        entity_pos_t & operator -= (const glm::vec3 & v) {
            offset -= v;
            glm::vec3 loffset = {
                16.0 * floorf(offset.x / 16.0),
                16.0 * floorf(offset.y / 16.0),
                16.0 * floorf(offset.z / 16.0)
            };
            chunkpos += loffset;
            offset -= loffset;
            return *this;
        }
        operator block_loc_t () {
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
