#include "window.hpp"
#include "config.hpp"
#include "handler.hpp"
#include "shader.hpp"
#include "terminal.hpp"
#include <algorithm>
#include <glm/gtc/matrix_transform.hpp>

namespace mineworld {
    void frame_buffer_size_callback(GLFWwindow *, int width, int height) {
        setting.FRAME_BUFFER_WIDTH = width;
        setting.FRAME_BUFFER_HEIGHT = height;
        glViewport(0,0,width,height);
        if (width <= 0 || height <= 0) return;
        handler.updatePerspectiveMat(glm::perspective(glm::radians(setting.FOV),
            static_cast<float>(width)/height, 0.1f,
            std::max(100.0f, (config.VISIBLE_DISTANCE+2)*16.0f)));
    }
    void window_size_callback(GLFWwindow *, int width, int height) {
        setting.WINDOW_WIDTH = width; setting.WINDOW_HEIGHT = height;
        setting.CENTER_WIDTH = width/2; setting.CENTER_HEIGHT = height/2;
        if (width <= 0 || height <= 0) return;
        gcrosshairshader.resize(width,height);
        gterminal.resize(); gboard.resize();
    }
    void installWindowCallbacks(GLFWwindow * window) {
        glfwSetWindowSizeCallback(window, window_size_callback);
        glfwSetFramebufferSizeCallback(window, frame_buffer_size_callback);
        glfwSetWindowFocusCallback(window, [](GLFWwindow *, int) { resetInput(); handler.timer.reset(); });
    }
    GLFWwindow * glfwStart() {
        if (!glfwInit()) { std::cerr << "[window] failed to initialize GLFW\n"; return nullptr; }
        glfwWindowHint(GLFW_SAMPLES,4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
        glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT,GL_TRUE);
        glfwWindowHint(GLFW_SCALE_TO_MONITOR,GLFW_TRUE);
        glfwWindowHint(GLFW_SCALE_FRAMEBUFFER,GLFW_TRUE);
        GLFWwindow * window = glfwCreateWindow(setting.WINDOW_WIDTH,setting.WINDOW_HEIGHT,"MineWorld",nullptr,nullptr);
        if (!window) { glfwTerminate(); return nullptr; }
        glfwMakeContextCurrent(window);
        glewExperimental = true;
        if (glewInit() != GLEW_OK) {
            glfwDestroyWindow(window); glfwTerminate();
            std::cerr << "[window] failed to initialize GLEW\n";
            return nullptr;
        }
        // Some GLEW versions probe extensions unavailable in a core context.
        glGetError();
        glfwSwapInterval(setting.VERTICAL_SYNC ? 1 : 0);
        glfwGetFramebufferSize(window,&setting.FRAME_BUFFER_WIDTH,&setting.FRAME_BUFFER_HEIGHT);
        glViewport(0,0,setting.FRAME_BUFFER_WIDTH,setting.FRAME_BUFFER_HEIGHT);
        glfwSetInputMode(window,GLFW_CURSOR,GLFW_CURSOR_DISABLED);
        glfwSetCursorPos(window,setting.CENTER_WIDTH,setting.CENTER_HEIGHT);
        glClearColor(0,0,1,1);
        glEnable(GL_DEPTH_TEST); glDepthFunc(GL_LEQUAL);
        glEnable(GL_CULL_FACE); glCullFace(GL_BACK);
        return window;
    }
}
