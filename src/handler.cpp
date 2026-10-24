#include "handler.hpp"
#include "config.hpp"
#include "cell.hpp"
#include "block.hpp"
#include "terminal.hpp"
#include "image.hpp"
#include <algorithm>
#include <ctime>

namespace mineworld {
    Handler handler;
    
    bool keypressed[349]; // max key num in glfw : 348
    void resetInput() { std::fill(std::begin(keypressed), std::end(keypressed), false); }
    
    void mouse_button_callback(GLFWwindow*, int button, int action, int) {
        if (action == GLFW_PRESS && handler.playstate == PLAYING) {
            if (!handler.player || handler.player->blockhitpos.face == NONE) return;
            const block_loc_t & blockloc = handler.player->hitblock;
            const hit_pos_t & hitpos = handler.player->blockhitpos;
            int holdblock = handler.player->getHoldBlock();
            int blockid = gchunk(blockloc);
            Block * hitblock = gblockregister.getBlockbyID(blockid);
            if (button == GLFW_MOUSE_BUTTON_RIGHT) {
                hitblock->rightClick(blockloc, hitpos, holdblock);
            }else if (button == GLFW_MOUSE_BUTTON_LEFT) {
                hitblock->leftClick(blockloc, hitpos, holdblock);
            }else if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
                handler.player->setHoldBlock(blockid);
            }
        }
    }
    
    void Handler::init(GLFWwindow * targetWindow, Player * targetPlayer) {
        window = targetWindow;
        glfwSetKeyCallback(window, KeyCallBack);
        glfwSetMouseButtonCallback(window, mouse_button_callback);
        player = targetPlayer;
        playstate = PLAYING;
        
        horizontalAngle = 0.0;
        verticalAngle = 0.0;
        
        mouseSpeed = setting.mouseSpeed;
        vMouseSpeed = setting.mouseSpeed;
        if (setting.INVERT_Y_AXIS) vMouseSpeed = -vMouseSpeed;
        
        runSpeed = setting.walkSpeed;
        
        ProjectionMatrix = glm::perspective(glm::radians(setting.FOV),
            static_cast<float>(setting.FRAME_BUFFER_WIDTH) / std::max(1,setting.FRAME_BUFFER_HEIGHT),
            0.1f, std::max(100.0f,(config.VISIBLE_DISTANCE+2)*16.0f));
        resetInput();
        timer.reset();
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
    
    void KeyCallBack(GLFWwindow * window, int key, int, int action, int){
        if (action == GLFW_PRESS) {
            if (key >= 0 && key <= GLFW_KEY_LAST)
                keypressed[key] = 1;
        }else if (action == GLFW_RELEASE) {
            if (key >= 0 && key <= GLFW_KEY_LAST)
                keypressed[key] = 0;
            switch (key) {
                case GLFW_KEY_LEFT:
                {
                    if (handler.playstate == PLAYING) handler.player->prevBlock();
                    break;
                }
                
                case GLFW_KEY_RIGHT:
                {
                    if (handler.playstate == PLAYING) handler.player->nextBlock();
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
                    if (handler.playstate == TYPING) gterminal.execute();
                    break;
                }
                
                case GLFW_KEY_BACKSPACE:
                {
                    if (handler.playstate == TYPING) gterminal.del();
                    break;
                }
                    
                case GLFW_KEY_F2:
                {
                    time_t abstime = time(0);
                    tm ltime{};
#ifdef _WIN32
                    localtime_s(&ltime, &abstime);
#else
                    localtime_r(&abstime, &ltime);
#endif
                    char buffer[128];
                    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d_%H.%M.%S.bmp", &ltime);
                    SOIL_save_screenshot(buffer, SOIL_SAVE_TYPE_BMP, 0, 0, setting.FRAME_BUFFER_WIDTH, setting.FRAME_BUFFER_HEIGHT);
                    std::cout << "screenshots saved as " << buffer << std::endl;
                    break;
                }
                    
                default:
                    break;
            } /* end switch */
        } /* end if */
    } /* KeyCallBack */
    
    void CharCallBack(GLFWwindow *, unsigned int c) {
        gterminal.inputc(c);
    }
    


}
