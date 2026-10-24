#ifndef handler_hpp
#define handler_hpp

#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "types.hpp"
#include "entity.hpp"
#include "timer.hpp"

namespace mineworld {
    
    class Player;
    
    class Handler {
    public:
        GLFWwindow * window = nullptr;
        Timer timer;
        
        Player * player = nullptr;
        PlayingState playstate = PAUSE;
        
        float horizontalAngle, verticalAngle;
        
        glm::mat4 ViewMatrix;
        glm::mat4 ProjectionMatrix;
        
        float mouseSpeed, vMouseSpeed;
        float runSpeed;
        
    public:
        void init(GLFWwindow * window, Player * player);
        void handle_input();
        friend void KeyCallBack(GLFWwindow * window, int key, int scancode, int action, int mods);
        friend void CharCallBack(GLFWwindow * window, unsigned int c);
        friend void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
        friend class Player;
        
        glm::mat4 & ViewMat() {
            return ViewMatrix;
        }
        
        glm::mat4 & ProjMat() {
            return ProjectionMatrix;
        }
        
        void updatePerspectiveMat(glm::mat4 && perspectiveMat);
        
    };
    
    void KeyCallBack(GLFWwindow * window, int key, int scancode, int action, int mods);
    void CharCallBack(GLFWwindow * window, unsigned int c);
    void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
    void resetInput();
    
    extern Handler handler;
}

#endif /* handler_hpp */
