#ifndef timer_hpp
#define timer_hpp

#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace mineworld {
    class Timer {
        double lastCommit, currentTime;
    public:
        float deltaTime;
        Timer() {
            lastCommit = 0.0;
            currentTime = 0.0;
        }
        void step() {
            lastCommit = currentTime;
            currentTime = glfwGetTime();
            deltaTime = currentTime - lastCommit;
        }
        void printFPS() {
            std::cout << "[FPS] " << 1 / deltaTime << '\n';
        }
    };
    
    extern Timer gtimer;
}

#endif /* timer_hpp */
