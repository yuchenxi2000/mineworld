#ifndef window_hpp
#define window_hpp

#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

/*
 * create window & context, make context current
 */
namespace mineworld2 {
    GLFWwindow * glfwStart();
}
#endif /* window_hpp */
