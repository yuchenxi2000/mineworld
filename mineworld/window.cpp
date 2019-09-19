#include "window.hpp"
#include "config.hpp"
#include "handler.hpp"
#include "shader.hpp"
#include "terminal.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
void frame_buffer_size_callback(GLFWwindow * window, int width, int height) {
    setting.FRAME_BUFFER_WIDTH = width;
    setting.FRAME_BUFFER_HEIGHT = height;
    mineworld2::handler.updatePerspectiveMat(glm::perspective(
                                                  setting.FOV,
                                                  GLfloat(width) / GLfloat(height),
                                                  0.1f,
                                                  100.0f
                                                  ));
}

void window_size_callback(GLFWwindow * window, int width, int height) {
    setting.WINDOW_WIDTH = width;
    setting.WINDOW_HEIGHT = height;
    setting.CENTER_WIDTH = width / 2;
    setting.CENTER_HEIGHT = height / 2;
    
    mineworld2::gcrosshairshader.resize(width, height);
    mineworld2::gterminal.resize();
    mineworld2::gboard.resize();
}

GLFWwindow * mineworld2::glfwStart() {
    if (!glfwInit()) {
        std::cerr << "[window] failed to init glfw" << std::endl;
    }
    
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, GLFW_TRUE);
    
    GLFWwindow * window = glfwCreateWindow(setting.WINDOW_WIDTH, setting.WINDOW_HEIGHT, "MineWorld", NULL, NULL);
    
    if (window == NULL) {
        std::cerr << "[window] failed to open window" << std::endl;
        glfwTerminate();
    }
    
    glfwMakeContextCurrent(window);
    glewExperimental = true;
    
    if (glewInit() != GLEW_OK) {
        std::cerr << "[window] failed to init glew" << std::endl;
        glfwTerminate();
    }
    
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    glfwPollEvents();
    glfwSetCursorPos(window, setting.CENTER_WIDTH, setting.CENTER_WIDTH);
    
    glClearColor(0.0, 0.0, 1.0, 1.0);
    
    // enable depth buffer
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    
    // cull back faces
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    
    // reset WINDOW_WIDTH, WINDOW_HEIGHT when they are changed
    glfwSetWindowSizeCallback(window, window_size_callback);
    glfwSetFramebufferSizeCallback(window, frame_buffer_size_callback);
    
    return window;
}
