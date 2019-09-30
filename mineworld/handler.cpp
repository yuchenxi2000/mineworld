#include "handler.hpp"
#include "config.hpp"

namespace mineworld {
    Handler handler;
    
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
        // TODO: initial position
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
            if (glfwGetKey(window, setting.KEY_FORWARD)) {
                player->entitypos += runSpeed * timer.deltaTime * forward;
            }
            if (glfwGetKey(window, setting.KEY_BACKWARD)) {
                player->entitypos -= runSpeed * timer.deltaTime * forward;
            }
            if (glfwGetKey(window, setting.KEY_LEFT)) {
                player->entitypos -= runSpeed * timer.deltaTime * right;
            }
            if (glfwGetKey(window, setting.KEY_RIGHT)) {
                player->entitypos += runSpeed * timer.deltaTime * right;
            }
            if (glfwGetKey(window, setting.KEY_UP)) {
                player->entitypos += runSpeed * timer.deltaTime * up;
            }
            if (glfwGetKey(window, setting.KEY_DOWN)) {
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
        
        if (key == GLFW_KEY_LEFT && action == GLFW_PRESS) {
            handler.player->prevBlock();
        }else if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS) {
            handler.player->nextBlock();
        }
        
        if (key == setting.KEY_ESC && action == GLFW_RELEASE) {
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
        }else if (key == setting.KEY_TYPE && action == GLFW_RELEASE) {
            switch (handler.playstate) {
                case PLAYING:
                case PAUSE:
                    handler.playstate = TYPING;
                    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                    std::cout << "[handler] typing" << std::endl;
                    glfwSetCharCallback(window, CharCallBack);
                    break;
                    
                default:
                    break;
            }
        }else if (key == GLFW_KEY_ENTER && action == GLFW_RELEASE) {
            gterminal.execute();
        }else if (key == GLFW_KEY_BACKSPACE && action == GLFW_RELEASE) {
            gterminal.del();
        }
        
    }
    
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
    
//    void Handler::getLookAt() {
//        state.lookAtBlock = 0;
//
//        double distance = DBL_MAX;
//        float horizontalAngle = state.horizontalAngle;
//        float verticalAngle = state.verticalAngle;
//
//        double x, y, z, t, xx, yy, zz;
//        xx = state.position.x;
//        yy = state.position.y;
//        zz = state.position.z;
//
//        // test y axis
//        int j = floor(yy);
//        bool positive = (verticalAngle > 0.0);
//        if (positive) {
//            j++;
//        }
//
//        if (0) {
//            //verticalAngle < 0.1 || verticalAngle > -0.1
//        }else{
//            while (true) {
//                t = (double(j) - yy) / state.lookDirection.y;
//
//                if (t >= 10.0 || t <= -10.0) {
//                    break;
//                }else{
//                    x = xx + state.lookDirection.x * t;
//                    z = zz + state.lookDirection.z * t;
//
//                    int position[3];
//                    position[0] = floor(x);
//                    if (positive) {
//                        position[1] = j;
//                    }else{
//                        position[1] = j - 1;
//                    }
//
//                    position[2] = floor(z);
//
//                    int block = gchunk(position[0] + state.chunkoffset.x, position[1] + state.chunkoffset.y, position[2] + state.chunkoffset.z);
//
//                    if (block != 0) {
//                        if (t < 0.0) {
//                            t = -t;
//                        }
//                        if (t < distance) {
//                            distance = t;
//                            state.lookAtPos.x = position[0];
//                            state.lookAtPos.y = position[1];
//                            state.lookAtPos.z = position[2];
//                            state.lookAtBlock = block;
//                            if (positive) {
//                                state.lookface = BOTTOM;
//                            }else{
//                                state.lookface = TOP;
//                            }
//                        }
//                        break;
//                    }
//
//                    if (positive) {
//                        j++;
//                    }else{
//                        j--;
//                    }
//                }
//
//            }
//
//        }
//
//
//
//        // test x axis
//        j = floor(xx);
//        positive = (sin(horizontalAngle) > 0.0);
//        if (positive) {
//            j++;
//        }
//
//        if (0) {
//            //sin(horizontalAngle) > 0.9 || sin(horizontalAngle) < -0.9
//        }else{
//            while (true) {
//                t = (double(j) - xx) / state.lookDirection.x;
//
//                if (t >= 10.0 || t <= -10.0) {
//                    break;
//                }else{
//                    y = yy + state.lookDirection.y * t;
//                    z = zz + state.lookDirection.z * t;
//
//                    int position[3];
//                    if (positive) {
//                        position[0] = j;
//                    }else{
//                        position[0] = j - 1;
//                    }
//
//                    position[1] = floor(y);
//                    position[2] = floor(z);
//
//                    int block = gchunk(position[0] + state.chunkoffset.x, position[1] + state.chunkoffset.y, position[2] + state.chunkoffset.z);
//
//                    if (block != 0) {
//                        if (t < 0.0) {
//                            t = -t;
//                        }
//                        if (t < distance) {
//                            distance = t;
//                            state.lookAtPos.x = position[0];
//                            state.lookAtPos.y = position[1];
//                            state.lookAtPos.z = position[2];
//                            state.lookAtBlock = block;
//                            if (positive) {
//                                state.lookface = RIGHT;
//                            }else{
//                                state.lookface = LEFT;
//                            }
//                        }
//                        break;
//                    }
//
//                    if (positive) {
//                        j++;
//                    }else{
//                        j--;
//                    }
//                }
//
//            }
//
//        }
//
//
//        // test z axis
//        j = floor(zz);
//        positive = (cos(horizontalAngle) > 0.0);
//        if (positive) {
//            j++;
//        }
//
//        if (0) {
//            //cos(horizontalAngle) > 0.9 || cos(horizontalAngle) < -0.9
//        }else{
//            while (true) {
//                t = (double(j) - zz) / state.lookDirection.z;
//
//                if (t >= 10.0 || t <= -10.0) {
//                    break;
//                }else{
//                    x = xx + state.lookDirection.x * t;
//                    y = yy + state.lookDirection.y * t;
//
//                    int position[3];
//                    position[0] = floor(x);
//                    position[1] = floor(y);
//                    if (positive) {
//                        position[2] = j;
//                    }else{
//                        position[2] = j - 1;
//                    }
//
//                    int block = gchunk(position[0] + state.chunkoffset.x, position[1] + state.chunkoffset.y, position[2] + state.chunkoffset.z);
//
//                    if (block != 0) {
//                        if (t < 0.0) {
//                            t = -t;
//                        }
//                        if (t < distance) {
//                            distance = t;
//                            state.lookAtPos.x = position[0];
//                            state.lookAtPos.y = position[1];
//                            state.lookAtPos.z = position[2];
//                            state.lookAtBlock = block;
//                            if (positive) {
//                                state.lookface = BACK;
//                            }else{
//                                state.lookface = FRONT;
//                            }
//                        }
//                        break;
//                    }
//
//                    if (positive) {
//                        j++;
//                    }else{
//                        j--;
//                    }
//                }
//
//            }
//
//        }
//
//        if (state.lookAtBlock == 0) {
//            state.lookface = NONE;
//        }
//    }
//
//    void Handler::printPosition() {
//        char buffer[64];
//        sprintf(buffer, "(%d, %d, %d)", (int)state.position.x + state.chunkoffset.x, (int)state.position.y + state.chunkoffset.y, (int)state.position.z + state.chunkoffset.z);
//        gterminal.println(std::string(buffer));
//    }
//
//    void Handler::printLookBlock() {
//        const char * face;
//        switch (state.lookface) {
//            case NONE:
//                face = "NONE";
//                break;
//
//            case TOP:
//                face = "TOP";
//                break;
//
//            case BOTTOM:
//                face = "BOTTOM";
//                break;
//
//            case LEFT:
//                face = "LEFT";
//                break;
//
//            case RIGHT:
//                face = "RIGHT";
//                break;
//
//            case FRONT:
//                face = "FRONT";
//                break;
//
//            case BACK:
//                face = "BACK";
//                break;
//
//            default:
//                break;
//        }
//        char buffer[128];
//        sprintf(buffer, "((%d, %d, %d), (%d, %s))", state.lookAtPos.x + state.chunkoffset.x, state.lookAtPos.y + state.chunkoffset.y, state.lookAtPos.z + state.chunkoffset.z, state.lookAtBlock, face);
//        gterminal.println(std::string(buffer));
//    }

}
