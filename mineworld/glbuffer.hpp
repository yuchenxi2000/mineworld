#ifndef glbuffer_hpp
#define glbuffer_hpp

#include <iostream>
#include <cstring>
#include <list>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iomanip>

/*
 * a simple interface for vertex buffer object
 *
 * insert vertices
 * must be called from main thread
 *
 */
namespace mineworld {
    
    typedef void SET_ATTRIB_PTR_FUNC();
    
    /*
     * class sv: rectangle storage
     *
     * example:
     * struct vert {
     *     float position[3];
     *     float uv[2];
     *     float normal[3];
     * }
     * struct rect {
     *     vert v[4];
     * }
     *
     */
    template <class sv, SET_ATTRIB_PTR_FUNC setattribptr>
    class glBuffer {
        static int indices_template[6];
        static GLuint ebo;
        static int ebo_length;
        
        static const int INDEX_STRIDE = sizeof(indices_template) / sizeof(int);
        static const int VERTEX_COUNT = 4;
        static const int INDEX_BYTES = sizeof(indices_template);
        
        static void ebo_create();
        static void ebo_expand(int capcity);
        
        GLuint vbo, vao;
        int vbo_length = 0;
    public:
        glBuffer();
        ~glBuffer();
        
        void insert(int count, sv * vertices);
        void clear();
        
        void draw();
    };
    
    template <class sv, SET_ATTRIB_PTR_FUNC setattribptr>
    int glBuffer<sv, setattribptr>::indices_template[6] = {
        0, 1, 2,
        2, 3, 0
    };
    
    template <class sv, SET_ATTRIB_PTR_FUNC setattribptr>
    GLuint glBuffer<sv, setattribptr>::ebo = 0;
    
    template <class sv, SET_ATTRIB_PTR_FUNC setattribptr>
    int glBuffer<sv, setattribptr>::ebo_length = 0;
    
    template <class sv, SET_ATTRIB_PTR_FUNC setattribptr>
    void glBuffer<sv, setattribptr>::ebo_create() {
        if (ebo == 0) {
            glGenBuffers(1, &ebo);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, 0, 0, GL_STATIC_DRAW);
        }
    }
    
    template <class sv, SET_ATTRIB_PTR_FUNC setattribptr>
    void glBuffer<sv, setattribptr>::ebo_expand(int capcity) {
        if (ebo_length < capcity) {
            int * elem = new int[capcity * INDEX_STRIDE];
            for (int i = 0; i < capcity; ++i) {
                for (int j = 0; j < INDEX_STRIDE; ++j) {
                    elem[i * INDEX_STRIDE + j] = indices_template[j] + i * VERTEX_COUNT;
                }
            }
            
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, capcity * INDEX_BYTES, elem, GL_STATIC_DRAW);
            ebo_length = capcity;
            delete [] elem;
        }
    }
    
    template <class sv, SET_ATTRIB_PTR_FUNC setattribptr>
    glBuffer<sv, setattribptr>::glBuffer() {
        ebo_create();
        
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);
        
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, 0, 0, GL_STATIC_DRAW);
        
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        
        setattribptr();
        
        glBindVertexArray(0);
    }
    
    template <class sv, SET_ATTRIB_PTR_FUNC setattribptr>
    glBuffer<sv, setattribptr>::~glBuffer() {
        glDeleteBuffers(1, &vbo);
        glDeleteVertexArrays(1, &vao);
    }
    
    template <class sv, SET_ATTRIB_PTR_FUNC setattribptr>
    void glBuffer<sv, setattribptr>::insert(int count, sv * vertices) {
        ebo_expand(count);
        vbo_length = count;
        
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, count * sizeof(sv), vertices, GL_STATIC_DRAW);
    }
    
    template <class sv, SET_ATTRIB_PTR_FUNC setattribptr>
    void glBuffer<sv, setattribptr>::clear() {
        insert(0, 0);
    }
    
    template <class sv, SET_ATTRIB_PTR_FUNC setattribptr>
    void glBuffer<sv, setattribptr>::draw() {
        if (vbo_length) {
            glBindVertexArray(vao);
            glDrawElements(GL_TRIANGLES, vbo_length * INDEX_STRIDE, GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);
        }
    }
    
}

#endif /* glbuffer_hpp */
