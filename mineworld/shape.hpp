#ifndef shape_hpp
#define shape_hpp

#include <iostream>
#include <iomanip>
#include <cstring>
#include "glbuffer.hpp"
#include "util.hpp"

/*
 * 定义了一些 OpenGL 的对象
 */
namespace mineworld2 {
    /*
     * 三维顶点、矩形
     */
    struct vertex {
        float position[3];
        float uv[2];
        float normal[3];
        
        vertex() {}
        vertex(const float * v) {
            memcpy(this, v, sizeof(vertex));
        }
        vertex & shift(const ivec3 & v) {
            position[0] += v.x;
            position[1] += v.y;
            position[2] += v.z;
            return *this;
        }
        vertex & setuv(int u, int v) {
            uv[0] = u; uv[1] = v;
            return *this;
        }
    };
    
    struct rect {
        vertex v[4];
        
        rect() {}
        rect(const float * v) {
            memcpy(this, v, sizeof(rect));
        }
        rect & shift(const ivec3 & offset) {
            v[0].position[0] += offset.x;
            v[0].position[1] += offset.y;
            v[0].position[2] += offset.z;
            v[1].position[0] += offset.x;
            v[1].position[1] += offset.y;
            v[1].position[2] += offset.z;
            v[2].position[0] += offset.x;
            v[2].position[1] += offset.y;
            v[2].position[2] += offset.z;
            v[3].position[0] += offset.x;
            v[3].position[1] += offset.y;
            v[3].position[2] += offset.z;
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
     * 二维顶点、矩形
     */
    struct vertex2D {
        float position[2];
        float uv[2];
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
            memcpy(this, f, sizeof(rect2D));
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
     * 顶点、矩形的平移函数
     */
    vertex shift(const vertex & v, const ivec3 & offset);
    rect shift(const rect & r, const ivec3 & offset);
    vertex2D shift(const vertex2D & v, float x, float y);
    rect2D shift(const rect2D & r, float x, float y);
    
    /*
     * 二维、三维矩形的缓冲对象
     */
    SET_ATTRIB_PTR_FUNC setrectattrib;
    typedef glBuffer<rect, setrectattrib> glRectBuffer;
    
    SET_ATTRIB_PTR_FUNC setrect2dattrib;
    typedef glBuffer<rect2D, setrect2dattrib> glRect2DBuffer;
}

#endif /* shape_hpp */
