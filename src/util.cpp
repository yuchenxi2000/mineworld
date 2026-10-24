#include "util.hpp"
#include <stdexcept>
namespace mineworld {
    std::string readFromFile(std::string path) {
        std::ifstream file(path);
        if (!file) throw std::runtime_error("cannot open file: " + path);
        std::ostringstream contents;
        contents << file.rdbuf();
        if (file.bad()) throw std::runtime_error("failed to read file: " + path);
        return contents.str();
    }
    GLuint compile(const char * vertexCode, const char * fragmentCode) {
        GLuint vertex = 0, fragment = 0, program = 0;
        auto stage = [](GLenum type, const char * code) {
            GLuint shader = glCreateShader(type);
            glShaderSource(shader, 1, &code, nullptr);
            glCompileShader(shader);
            GLint success = 0;
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success) {
                char log[1024]{};
                glGetShaderInfoLog(shader, sizeof(log), nullptr, log);
                glDeleteShader(shader);
                throw std::runtime_error(std::string("shader compilation failed: ") + log);
            }
            return shader;
        };
        try {
            vertex = stage(GL_VERTEX_SHADER, vertexCode);
            fragment = stage(GL_FRAGMENT_SHADER, fragmentCode);
            program = glCreateProgram();
            glAttachShader(program,vertex); glAttachShader(program,fragment);
            glLinkProgram(program);
            GLint success = 0;
            glGetProgramiv(program, GL_LINK_STATUS, &success);
            if (!success) {
                char log[1024]{};
                glGetProgramInfoLog(program,sizeof(log),nullptr,log);
                throw std::runtime_error(std::string("shader link failed: ") + log);
            }
            glDetachShader(program,vertex); glDetachShader(program,fragment);
            glDeleteShader(vertex); glDeleteShader(fragment);
            return program;
        } catch (...) {
            if (vertex) glDeleteShader(vertex);
            if (fragment) glDeleteShader(fragment);
            if (program) glDeleteProgram(program);
            throw;
        }
    }
}
