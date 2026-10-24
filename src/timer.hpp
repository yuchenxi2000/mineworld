#ifndef timer_hpp
#define timer_hpp
#include <algorithm>
#include <iostream>
#include <GLFW/glfw3.h>
namespace mineworld {
    class Timer {
        double currentTime = 0;
    public:
        float deltaTime = 0;
        void reset() { currentTime = glfwGetTime(); deltaTime = 0; }
        void step() {
            double now = glfwGetTime();
            deltaTime = static_cast<float>(std::clamp(now-currentTime,0.0,0.1));
            currentTime = now;
        }
        void printFPS() const { if (deltaTime > 0) std::cout << "[FPS] " << 1/deltaTime << '\n'; }
    };
    extern Timer gtimer;
}
#endif
