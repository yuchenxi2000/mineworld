#include "shape.hpp"
namespace mineworld2 {
    vertex shift(const vertex & v, const ivec3 & offset) {
        return vertex(v).shift(offset);
    }
    
    rect shift(const rect & r, const ivec3 & offset) {
        return rect(r).shift(offset);
    }
    
    vertex2D shift(const vertex2D & v, float x, float y) {
        return vertex2D(v).shift(x, y);
    }
    
    rect2D shift(const rect2D & r, float x, float y) {
        return rect2D(r).shift(x, y);
    }
    
    void setrectattrib() {
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
        glEnableVertexAttribArray(0);
        
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(1);
        
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(5 * sizeof(GLfloat)));
        glEnableVertexAttribArray(2);
    }
    
    void setrect2dattrib() {
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);
        glEnableVertexAttribArray(0);
        
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)(2 * sizeof(GLfloat)));
        glEnableVertexAttribArray(1);
    }
}
