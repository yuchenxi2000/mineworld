#include "util.hpp"
namespace mineworld {
    
    /*
     * read contents from file,
     * store them in std::string
     */
    std::string readFromFile(std::string path) {
        std::ifstream fs(path);
        std::stringstream ss;
        ss << fs.rdbuf();
        return ss.str();
    }
    
    /*
     * GLSL compiler
     * returns program id if success
     * call exit(-1) if failed
     */
    GLuint compile(const char * vShaderCode, const char * fShaderCode) {
        GLuint vertex, fragment;
        GLint success;
        GLchar infoLog[512];
        
        // vertex shader
        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vShaderCode, NULL);
        glCompileShader(vertex);
        // get and print error (if any)
        glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
        if(!success)
        {
            glGetShaderInfoLog(vertex, 512, NULL, infoLog);
            std::cerr << "[compiler] vertex compile failed\n" << infoLog << std::endl;
            exit(-1);
        };
        
        // same with fragment shader
        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fShaderCode, NULL);
        glCompileShader(fragment);
        // get and print error (if any)
        glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
        if(!success)
        {
            glGetShaderInfoLog(fragment, 512, NULL, infoLog);
            std::cerr << "[compiler] fragment compile failed\n" << infoLog << std::endl;
            exit(-1);
        };
        
        // shader program
        GLuint Program = glCreateProgram();
        glAttachShader(Program, vertex);
        glAttachShader(Program, fragment);
        glLinkProgram(Program);
        // get and print link error (if any)
        glGetProgramiv(Program, GL_LINK_STATUS, &success);
        if(!success)
        {
            glGetProgramInfoLog(Program, 512, NULL, infoLog);
            std::cerr << "[compiler] link failed\n" << infoLog << std::endl;
            exit(-1);
        }
        
        // delete vertex, fragment shader, they are no longer used.
        glDetachShader(Program, vertex);
        glDetachShader(Program, fragment);
        glDeleteShader(vertex);
        glDeleteShader(fragment);
        std::cout << "[compiler] compile success" << std::endl;
        
        return Program;
    }
    
}
