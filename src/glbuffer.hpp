#ifndef glbuffer_hpp
#define glbuffer_hpp
#include <vector>
#include <GL/glew.h>

namespace mineworld {
    typedef void SET_ATTRIB_PTR_FUNC();
    // All instances are created and destroyed with the owning GL context current.
    template<class sv, SET_ATTRIB_PTR_FUNC setattribptr>
    class glBuffer {
        inline static GLuint ebo = 0;
        inline static int eboLength = 0;
        inline static unsigned instances = 0;
        GLuint vbo = 0, vao = 0;
        int length = 0;
        static void expandIndices(int count) {
            if (count <= eboLength) return;
            std::vector<GLuint> indices(static_cast<size_t>(count) * 6);
            constexpr GLuint pattern[] = {0,1,2,2,3,0};
            for (int i = 0; i < count; ++i)
                for (int j = 0; j < 6; ++j) indices[6*i+j] = pattern[j] + 4*static_cast<GLuint>(i);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(GLuint), indices.data(), GL_STATIC_DRAW);
            eboLength = count;
        }
    public:
        glBuffer() {
            glGenVertexArrays(1, &vao);
            glBindVertexArray(vao);
            if (!ebo) glGenBuffers(1, &ebo);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
            glGenBuffers(1, &vbo);
            glBindBuffer(GL_ARRAY_BUFFER, vbo);
            glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_STATIC_DRAW);
            setattribptr();
            glBindVertexArray(0);
            ++instances;
        }
        glBuffer(const glBuffer &) = delete;
        glBuffer & operator=(const glBuffer &) = delete;
        ~glBuffer() {
            glDeleteBuffers(1, &vbo);
            glDeleteVertexArrays(1, &vao);
            if (--instances == 0) {
                glDeleteBuffers(1, &ebo);
                ebo = 0; eboLength = 0;
            }
        }
        void insert(int count, const sv * vertices) {
            glBindVertexArray(vao);
            expandIndices(count);
            glBindBuffer(GL_ARRAY_BUFFER, vbo);
            glBufferData(GL_ARRAY_BUFFER, static_cast<size_t>(count)*sizeof(sv), count ? vertices : nullptr, GL_STATIC_DRAW);
            glBindVertexArray(0);
            length = count;
        }
        void clear() { insert(0, nullptr); }
        int size() const { return length; }
        void draw() const {
            if (!length) return;
            glBindVertexArray(vao);
            glDrawElements(GL_TRIANGLES, length*6, GL_UNSIGNED_INT, nullptr);
            glBindVertexArray(0);
        }
    };
}
#endif
