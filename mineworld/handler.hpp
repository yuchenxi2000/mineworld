#ifndef handler_hpp
#define handler_hpp

#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "util.hpp"
#include "cell.hpp"
#include "entity.hpp"
#include "terminal.hpp"

namespace mineworld2 {
    struct GameState {
        enum {
            LOGIN,
            PLAYING,
            PAUSE,
            TYPING
        } PlayingState;
        
        // bool pressedKeys[385]; // max key num in glfw : 384
        
        enum {
            NONE,
            LEFT,
            RIGHT,
            TOP,
            BOTTOM,
            FRONT,
            BACK
        } lookFace;
        mineworld2::ivec3 lookAtPos;
        int lookAtBlock;
        
        double lastCommit, currentTime;
        float runSpeed, mouseSpeed, vMouseSpeed;
        
        double scrollSpeed;
        double scrollY;
        
        float horizontalAngle, verticalAngle;
        glm::vec3 lookDirection;
        
        ivec3 chunkoffset;
        glm::vec3 position;
        
        glm::mat4 ViewMatrix;
        glm::mat4 ProjectionMatrix;
        
        void init();
    };
    
    class Handler {
    public:
        GameState state;
        GLFWwindow * window;
    public:
        void init(GLFWwindow * window);
        void handle_input();
        friend void KeyCallBack(GLFWwindow * window, int key, int scancode, int action, int mods);
        friend void CharCallBack(GLFWwindow * window, unsigned int c);
        friend void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
        
        ivec3 getPosition() {
            return ivec3(
                         state.chunkoffset.x + (int)state.position.x,
                         state.chunkoffset.y + (int)state.position.y,
                         state.chunkoffset.z + (int)state.position.z
                         );
        }
        
        void setPosition(const ivec3 & pos) {
            state.chunkoffset.x = floor16(pos.x);
            state.chunkoffset.y = floor16(pos.y);
            state.chunkoffset.z = floor16(pos.z);
            
            state.position.x = pos.x - state.chunkoffset.x + 0.5;
            state.position.y = pos.y - state.chunkoffset.y + 3.0;
            state.position.z = pos.z - state.chunkoffset.z + 0.5;
        }
        
        glm::mat4 & ViewMat() {
            return state.ViewMatrix;
        }
        
        glm::mat4 & ProjMat() {
            return state.ProjectionMatrix;
        }
        
        void updatePerspectiveMat(glm::mat4 && perspectiveMat);
        
        // calculate which block player is looking at
        void getLookAt();
        
        void printPosition();
        void printLookBlock();
        
    };
    
    void KeyCallBack(GLFWwindow * window, int key, int scancode, int action, int mods);
    void CharCallBack(GLFWwindow * window, unsigned int c);
    void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
    void scroll_callback(GLFWwindow * window, double x, double y);
    
    extern Handler handler;
}

#endif /* handler_hpp */
