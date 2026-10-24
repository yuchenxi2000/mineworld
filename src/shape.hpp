#ifndef shape_hpp
#define shape_hpp

#include <iostream>
#include <iomanip>
#include <cstring>
#include "glbuffer.hpp"
#include "util.hpp"

/*
 * some OpenGL objects
 */
namespace mineworld {
    /*
     * vertex, rectangle in 3 dimension
     */
    struct vertex {
        glm::vec3 position{0};
        glm::vec2 uv{0};
        glm::vec3 normal{0};
        
        vertex() {}
        vertex(const float * v) {
            position = {v[0],v[1],v[2]};
            uv = {v[3],v[4]};
            normal = {v[5],v[6],v[7]};
        }
        vertex & shift(const glm::ivec3 & v) {
            position += v;
            return *this;
        }
        vertex & setuv(float u, float v) {
            uv[0] = u; uv[1] = v;
            return *this;
        }
    };
    
    struct rect {
        vertex v[4];
        
        rect() {}
        rect(const float * v) {
            for (int i = 0; i < 4; ++i) this->v[i] = vertex(v + 8*i);
        }
        rect & shift(const glm::ivec3 & offset) {
            v[0].position += offset;
            v[1].position += offset;
            v[2].position += offset;
            v[3].position += offset;
            return *this;
        }
        void out(float x) {
            std::cout << x << '\t';
        }
        void print() {
            for (int i = 0; i < 4; ++i) {
                out(v[i].position[0]);
                out(v[i].position[1]);
                out(v[i].position[2]);
                out(v[i].uv[0]);
                out(v[i].uv[1]);
                std::cout << std::endl;
            }
        }
    };
    
    /*
     * vertex, rectangle in 2 dimension
     */
    struct vertex2D {
        glm::vec2 position{0};
        glm::vec2 uv{0};
        vertex2D & shift(float x, float y) {
            position[0] += x;
            position[1] += y;
            return *this;
        }
    };
    
    struct rect2D {
        vertex2D v[4];
        rect2D() {}
        rect2D(const float * f) {
            for (int i = 0; i < 4; ++i) {
                v[i].position = {f[4*i], f[4*i+1]};
                v[i].uv = {f[4*i+2], f[4*i+3]};
            }
        }
        void setposition(float x, float y, float width, float height) {
            v[0].position[0] = x;
            v[0].position[1] = y;
            v[1].position[0] = x;
            v[1].position[1] = y - height;
            v[2].position[0] = x + width;
            v[2].position[1] = y - height;
            v[3].position[0] = x + width;
            v[3].position[1] = y;
        }
        void setuv(float x, float y, float width, float height) {
            v[0].uv[0] = x;
            v[0].uv[1] = y;
            v[1].uv[0] = x;
            v[1].uv[1] = y + height;
            v[2].uv[0] = x + width;
            v[2].uv[1] = y + height;
            v[3].uv[0] = x + width;
            v[3].uv[1] = y;
        }
        rect2D & shift(float x, float y) {
            v[0].position[0] += x;
            v[1].position[0] += x;
            v[2].position[0] += x;
            v[3].position[0] += x;
            v[0].position[1] += y;
            v[1].position[1] += y;
            v[2].position[1] += y;
            v[3].position[1] += y;
            return *this;
        }
    };
    
    /*
     * move in one direction
     */
    vertex shift(const vertex & v, const glm::ivec3 & offset);
    rect shift(const rect & r, const glm::ivec3 & offset);
    vertex2D shift(const vertex2D & v, float x, float y);
    rect2D shift(const rect2D & r, float x, float y);
    
    /*
     * vertex buffer obj for 2D, 3D vertex, rectangle
     */
    SET_ATTRIB_PTR_FUNC setrectattrib;
    typedef glBuffer<rect, setrectattrib> glRectBuffer;
    
    SET_ATTRIB_PTR_FUNC setrect2dattrib;
    typedef glBuffer<rect2D, setrect2dattrib> glRect2DBuffer;
}

#endif /* shape_hpp */
