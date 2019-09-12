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

namespace mineworld2 {
    struct ivec3 {
        int x, y, z;
        ivec3() {
            x = 0; y = 0; z = 0;
        }
        ivec3(int x, int y, int z) {
            this->x = x; this->y = y; this->z = z;
        }
        ~ivec3() {}
        ivec3 & operator () (int x, int y, int z) {
            this->x = x; this->y = y; this->z = z;
            return *this;
        }
        bool operator == (const ivec3 & v) const {
            return (this->x == v.x) && (this->y == v.y) && (this->z == v.z);
        }
        bool operator != (const ivec3 & v) const {
            return (this->x != v.x) || (this->y != v.y) || (this->z != v.z);
        }
        ivec3 operator - (const ivec3 & b) {
            return ivec3(this->x - b.x, this->y - b.y, this->z - b.z);
        }
        ivec3 operator + (const ivec3 & b) {
            return ivec3(this->x + b.x, this->y + b.y, this->z + b.z);
        }
        ivec3 & operator += (const ivec3 & b) {
            x += b.x; y += b.y; z += b.z;
            return *this;
        }
        ivec3 & operator -= (const ivec3 & b) {
            x -= b.x; y -= b.y; z -= b.z;
            return *this;
        }
    };
    
    struct ivec3_hash {
        size_t operator () (const ivec3 & v) const {
            size_t x = v.x;
            x = (x << 32) + v.z;
            size_t y = v.y;
            x ^= y << 16;
            return std::hash<size_t> () (x);
        }
    };
    int ivec3_distance0(const ivec3 & a, const ivec3 & b);
    
    struct block_loc_t {
        ivec3 chunkpos;
        ivec3 blockoffset;
    };
    
    block_loc_t getBlockInChunk(const ivec3 & pos);
    
    int floor16(int x);
    
    std::string readFromFile(std::string path);
    
    GLuint compile(const char * vShaderCode, const char * fShaderCode);
    
}

#endif /* util_hpp */
