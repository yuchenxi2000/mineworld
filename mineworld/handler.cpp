#include "handler.hpp"
#include "config.hpp"

namespace mineworld {
    Handler handler;
    
    bool keypressed[349]; // max key num in glfw : 348
    
    void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
        if (action == GLFW_PRESS && handler.playstate == PLAYING) {
            auto & blockloc = handler.player->hitblock;
            glm::ivec3 pos = blockloc.chunkpos + blockloc.offset;
            // TODO
            int holdblock = handler.player->getHoldBlock();
            if (button == GLFW_MOUSE_BUTTON_RIGHT) {
                gblockregister.getBlockbyID(holdblock)->rightClick(blockloc, handler.player->blockhitpos, holdblock);
            }else if (button == GLFW_MOUSE_BUTTON_LEFT) {
                gblockregister.getBlockbyID(holdblock)->leftClick(pos, handler.player->blockhitpos, holdblock);
            }
        }
    }
    
    void Handler::init(GLFWwindow * window, Player * player) {
        this->window = window;
        glfwSetKeyCallback(window, KeyCallBack);
        glfwSetMouseButtonCallback(window, mouse_button_callback);
        // TODO
//        glfwSetScrollCallback(window, scroll_callback);
        this->player = player;
        playstate = PLAYING;
        
        horizontalAngle = 0.0;
        verticalAngle = 0.0;
        
        mouseSpeed = setting.mouseSpeed;
        vMouseSpeed = setting.mouseSpeed;
        if (setting.INVERT_Y_AXIS) vMouseSpeed = -vMouseSpeed;
        
        runSpeed = setting.walkSpeed;
        
        ProjectionMatrix = glm::perspective(setting.FOV, (float)setting.FRAME_BUFFER_WIDTH / (float)setting.FRAME_BUFFER_HEIGHT, 0.1f, 100.0f);
    }
    
    void Handler::handle_input() {
        timer.step();
        if (setting.SHOW_FPS) timer.printFPS();
        
        if (playstate == PLAYING) {
            double xpos, ypos;
            glfwGetCursorPos(window, &xpos, &ypos);
            glfwSetCursorPos(window, setting.CENTER_WIDTH, setting.CENTER_HEIGHT);
            
            horizontalAngle += mouseSpeed * float(setting.CENTER_WIDTH - xpos);
            verticalAngle   += vMouseSpeed * float(setting.CENTER_HEIGHT - ypos);
            
            if (verticalAngle > 1.57f) {
                verticalAngle = 1.57f;
            }
            
            if (verticalAngle < -1.57f) {
                verticalAngle = -1.57f;
            }
            
            glm::vec3 right = glm::vec3(-cos(horizontalAngle), 0, sin(horizontalAngle));
            glm::vec3 up(0.0f, 1.0f, 0.0f);
            glm::vec3 forward(sin(horizontalAngle), 0, cos(horizontalAngle));
            if (keypressed[GLFW_KEY_W]) {
                player->entitypos += runSpeed * timer.deltaTime * forward;
            }
            if (keypressed[GLFW_KEY_S]) {
                player->entitypos -= runSpeed * timer.deltaTime * forward;
            }
            if (keypressed[GLFW_KEY_A]) {
                player->entitypos -= runSpeed * timer.deltaTime * right;
            }
            if (keypressed[GLFW_KEY_D]) {
                player->entitypos += runSpeed * timer.deltaTime * right;
            }
            if (keypressed[GLFW_KEY_SPACE]) {
                player->entitypos += runSpeed * timer.deltaTime * up;
            }
            if (keypressed[GLFW_KEY_LEFT_SHIFT]) {
                player->entitypos -= runSpeed * timer.deltaTime * up;
            }
            player->lookdirection = glm::vec3(
                                            cos(verticalAngle) * sin(horizontalAngle),
                                            sin(verticalAngle),
                                            cos(verticalAngle) * cos(horizontalAngle)
                                            );
            ViewMatrix = glm::lookAt(glm::vec3(0, 0, 0), player->lookdirection, up);
            
        }
        
        player->updateLookAt();
        
    }
    void Handler::updatePerspectiveMat(glm::mat4 && perspectiveMat) {
        this->ProjectionMatrix = perspectiveMat;
    }
    
    void KeyCallBack(GLFWwindow * window, int key, int scancode, int action, int mods){
        if (action == GLFW_PRESS) {
            if (key >= 0)
                keypressed[key] = 1;
        }else if (action == GLFW_RELEASE) {
            if (key >= 0)
                keypressed[key] = 0;
            switch (key) {
                case GLFW_KEY_LEFT:
                {
                    handler.player->prevBlock();
                    break;
                }
                
                case GLFW_KEY_RIGHT:
                {
                    handler.player->nextBlock();
                    break;
                }
                
                case GLFW_KEY_ESCAPE:
                {
                    switch (handler.playstate) {
                        case PLAYING:
                            handler.playstate = PAUSE;
                            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                            std::cout << "[handler] game paused" << std::endl;
                            break;
                            
                        case TYPING:
                            handler.playstate = PLAYING;
                            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                            std::cout << "[handler] start playing" << std::endl;
                            glfwSetCursorPos(window, setting.CENTER_WIDTH, setting.CENTER_HEIGHT);
                            glfwSetCharCallback(window, 0);
                            break;
                            
                        case PAUSE:
                            handler.playstate = PLAYING;
                            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                            std::cout << "[handler] start playing" << std::endl;
                            glfwSetCursorPos(window, setting.CENTER_WIDTH, setting.CENTER_HEIGHT);
                            break;
                            
                        default:
                            break;
                    }
                    break;
                }
                
                case GLFW_KEY_T:
                {
                    switch (handler.playstate) {
                        case PLAYING:
                        case PAUSE:
                            handler.playstate = TYPING;
                            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                            glfwSetCharCallback(window, CharCallBack);
                            std::cout << "[handler] typing" << std::endl;
                            break;
                            
                        default:
                            break;
                    }
                    break;
                }
                
                case GLFW_KEY_ENTER:
                {
                    gterminal.execute();
                    break;
                }
                
                case GLFW_KEY_BACKSPACE:
                {
                    gterminal.del();
                    break;
                }
                    
                default:
                    break;
            } /* end switch */
        } /* end if */
    } /* KeyCallBack */
    
    void CharCallBack(GLFWwindow * window, unsigned int c) {
        gterminal.inputc(c);
    }
    
//    void scroll_callback(GLFWwindow * window, double x, double y) {
//        handler.state.scrollY += y;
//        if (handler.state.scrollY >= handler.state.scrollSpeed) {
//            mineworld2::gplayer.nextBlock();
//            handler.state.scrollY = -handler.state.scrollSpeed;
//        }else if (handler.state.scrollY <= -handler.state.scrollSpeed) {
//            mineworld2::gplayer.prevBlock();
//            handler.state.scrollY = handler.state.scrollSpeed;
//        }
//    }

}
