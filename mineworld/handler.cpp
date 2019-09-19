#include "handler.hpp"
#include "config.hpp"

namespace mineworld2 {
    Handler handler;
    
    void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
        if (action == GLFW_PRESS && handler.state.PlayingState == GameState::PLAYING) {
            ivec3 pos = handler.state.lookAtPos + handler.state.chunkoffset;
            int holdblock = gplayer.getHoldBlock();
            if (button == GLFW_MOUSE_BUTTON_RIGHT) {
                if (holdblock != 0) {
                    switch (handler.state.lookFace) {
                        case mineworld2::GameState::TOP:
                            pos.y++;
                            break;
                            
                        case mineworld2::GameState::BOTTOM:
                            pos.y--;
                            break;
                            
                        case mineworld2::GameState::LEFT:
                            pos.x++;
                            break;
                            
                        case mineworld2::GameState::RIGHT:
                            pos.x--;
                            break;
                            
                        case mineworld2::GameState::FRONT:
                            pos.z++;
                            break;
                            
                        case mineworld2::GameState::BACK:
                            pos.z--;
                            break;
                            
                        case mineworld2::GameState::NONE:
                            return;
                            
                        default:
                            break;
                    }
                    gchunk.blockUpdate(pos, holdblock);
                }
            }else if (button == GLFW_MOUSE_BUTTON_LEFT) {
                if (handler.state.lookFace != mineworld2::GameState::NONE)
                    gchunk.blockUpdate(pos, 0);
            }
        }
    }
    
    void GameState::init() {
        this->PlayingState = GameState::PLAYING;
        this->lastCommit = glfwGetTime();
        this->currentTime = lastCommit;
        this->horizontalAngle = 0.0;
        this->verticalAngle = 0.0;
        this->mouseSpeed = setting.mouseSpeed;
        this->vMouseSpeed = setting.mouseSpeed;
        if (setting.INVERT_Y_AXIS) vMouseSpeed = -vMouseSpeed;
        this->runSpeed = setting.walkSpeed;
        this->scrollSpeed = 5;
        this->scrollY = 0;
        
        block_loc_t bl = getBlockInChunk(setting.initialPos);
        this->chunkoffset = bl.chunkpos;
        this->position = glm::vec3(bl.blockoffset.x + 0.5, bl.blockoffset.y, bl.blockoffset.z + 0.5);
        
        this->ProjectionMatrix = glm::perspective(
                                                  setting.FOV,
                                                  GLfloat(setting.WINDOW_WIDTH) / GLfloat(setting.WINDOW_HEIGHT),
                                                  0.1f,
                                                  100.0f
                                                  );
        this->ViewMatrix[0][0] = 1;
        this->ViewMatrix[1][1] = 1;
        this->ViewMatrix[2][2] = 1;
        this->ViewMatrix[3][3] = 1;
    }
    
    void Handler::init(GLFWwindow * window) {
        this->state.init();
        this->window = window;
        glfwSetKeyCallback(window, KeyCallBack);
        glfwSetMouseButtonCallback(window, mouse_button_callback);
        glfwSetScrollCallback(window, scroll_callback);
    }
    
    void Handler::handle_input() {
        state.currentTime = glfwGetTime();
        float deltaTime = float(state.currentTime - state.lastCommit);
        state.lastCommit = state.currentTime;
        
        if (setting.SHOW_FPS) {
            printf("[FPS] %f\n", 1 / deltaTime);
        }
        
        if (state.PlayingState == GameState::PLAYING) {
            double xpos, ypos;
            glfwGetCursorPos(window, &xpos, &ypos);
            glfwSetCursorPos(window, setting.CENTER_WIDTH, setting.CENTER_HEIGHT);
            
            state.horizontalAngle += state.mouseSpeed * float(setting.CENTER_WIDTH - xpos);
            state.verticalAngle   += state.vMouseSpeed * float(setting.CENTER_HEIGHT - ypos);
            
            if (state.verticalAngle > 1.57f) {
                state.verticalAngle = 1.57f;
            }
            
            if (state.verticalAngle < -1.57f) {
                state.verticalAngle = -1.57f;
            }
            
            glm::vec3 right = glm::vec3(-cos(state.horizontalAngle), 0, sin(state.horizontalAngle));
            glm::vec3 up(0.0f, 1.0f, 0.0f);
            glm::vec3 forward(sin(state.horizontalAngle), 0, cos(state.horizontalAngle));
            if (glfwGetKey(window, setting.KEY_FORWARD)) {
                state.position += forward * state.runSpeed * deltaTime;
            }
            if (glfwGetKey(window, setting.KEY_BACKWARD)) {
                state.position -= forward * state.runSpeed * deltaTime;
            }
            if (glfwGetKey(window, setting.KEY_LEFT)) {
                state.position -= right * state.runSpeed * deltaTime;
            }
            if (glfwGetKey(window, setting.KEY_RIGHT)) {
                state.position += right * state.runSpeed * deltaTime;
            }
            if (glfwGetKey(window, setting.KEY_UP)) {
                state.position += up * state.runSpeed * deltaTime;
            }
            if (glfwGetKey(window, setting.KEY_DOWN)) {
                state.position -= up * state.runSpeed * deltaTime;
            }
            if (state.position.x >= CELL_X) {
                state.position.x -= CELL_X;
                state.chunkoffset.x += CELL_X;
            }
            if (state.position.y >= CELL_Y) {
                state.position.y -= CELL_Y;
                state.chunkoffset.y += CELL_Y;
            }
            if (state.position.z >= CELL_Z) {
                state.position.z -= CELL_Z;
                state.chunkoffset.z += CELL_Z;
            }
            if (state.position.x < 0) {
                state.position.x += CELL_X;
                state.chunkoffset.x -= CELL_X;
            }
            if (state.position.y < 0) {
                state.position.y += CELL_Y;
                state.chunkoffset.y -= CELL_Y;
            }
            if (state.position.z < 0) {
                state.position.z += CELL_Z;
                state.chunkoffset.z -= CELL_Z;
            }
            
            state.lookDirection = glm::vec3(
                                            cos(state.verticalAngle) * sin(state.horizontalAngle),
                                            sin(state.verticalAngle),
                                            cos(state.verticalAngle) * cos(state.horizontalAngle)
                                            );
            state.ViewMatrix = glm::lookAt(glm::vec3(0), state.lookDirection, up);
            
        }
        
        getLookAt();
        
    }
    void Handler::updatePerspectiveMat(glm::mat4 && perspectiveMat) {
        this->state.ProjectionMatrix = perspectiveMat;
    }
    
    void KeyCallBack(GLFWwindow * window, int key, int scancode, int action, int mods){
        
        if (key == setting.KEY_ESC && action == GLFW_RELEASE) {
            switch (handler.state.PlayingState) {
                case GameState::PLAYING:
                    handler.state.PlayingState = GameState::PAUSE;
                    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                    std::cout << "[handler] game paused" << std::endl;
                    break;
                    
                case GameState::TYPING:
                    handler.state.PlayingState = GameState::PLAYING;
                    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                    std::cout << "[handler] start playing" << std::endl;
                    glfwSetCursorPos(window, setting.CENTER_WIDTH, setting.CENTER_HEIGHT);
                    glfwSetCharCallback(window, 0);
                    break;
                    
                case GameState::PAUSE:
                    handler.state.PlayingState = GameState::PLAYING;
                    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                    std::cout << "[handler] start playing" << std::endl;
                    glfwSetCursorPos(window, setting.CENTER_WIDTH, setting.CENTER_HEIGHT);
                    break;
                    
                default:
                    break;
            }
        }else if (key == setting.KEY_TYPE && action == GLFW_RELEASE) {
            switch (handler.state.PlayingState) {
                case GameState::PLAYING:
                case GameState::PAUSE:
                    handler.state.PlayingState = GameState::TYPING;
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
    
    void scroll_callback(GLFWwindow * window, double x, double y) {
        handler.state.scrollY += y;
        if (handler.state.scrollY >= handler.state.scrollSpeed) {
            mineworld2::gplayer.nextBlock();
            handler.state.scrollY = -handler.state.scrollSpeed;
        }else if (handler.state.scrollY <= -handler.state.scrollSpeed) {
            mineworld2::gplayer.prevBlock();
            handler.state.scrollY = handler.state.scrollSpeed;
        }
    }
    
    void Handler::getLookAt() {
        state.lookAtBlock = 0;
        
        double distance = DBL_MAX;
        float horizontalAngle = state.horizontalAngle;
        float verticalAngle = state.verticalAngle;
        
        double x, y, z, t, xx, yy, zz;
        xx = state.position.x;
        yy = state.position.y;
        zz = state.position.z;
        
        // test y axis
        int j = floor(yy);
        bool positive = (verticalAngle > 0.0);
        if (positive) {
            j++;
        }
        
        if (0) {
            //verticalAngle < 0.1 || verticalAngle > -0.1
        }else{
            while (true) {
                t = (double(j) - yy) / sin(verticalAngle);
                
                if (t >= 10.0 || t <= -10.0) {
                    break;
                }else{
                    x = xx + cos(verticalAngle) * sin(horizontalAngle) * t;
                    z = zz + cos(verticalAngle) * cos(horizontalAngle) * t;
                    
                    int position[3];
                    position[0] = floor(x);
                    if (positive) {
                        position[1] = j;
                    }else{
                        position[1] = j - 1;
                    }
                    
                    position[2] = floor(z);
                    
                    int block = gchunk(position[0] + state.chunkoffset.x, position[1] + state.chunkoffset.y, position[2] + state.chunkoffset.z);
                    
                    if (block != 0) {
                        if (t < 0.0) {
                            t = -t;
                        }
                        if (t < distance) {
                            distance = t;
                            state.lookAtPos.x = position[0];
                            state.lookAtPos.y = position[1];
                            state.lookAtPos.z = position[2];
                            state.lookAtBlock = block;
                            if (positive) {
                                state.lookFace = GameState::BOTTOM;
                            }else{
                                state.lookFace = GameState::TOP;
                            }
                        }
                        break;
                    }
                    
                    if (positive) {
                        j++;
                    }else{
                        j--;
                    }
                }
                
            }
            
        }
        
        
        
        // test x axis
        j = floor(xx);
        positive = (sin(horizontalAngle) > 0.0);
        if (positive) {
            j++;
        }
        
        if (0) {
            //sin(horizontalAngle) > 0.9 || sin(horizontalAngle) < -0.9
        }else{
            while (true) {
                t = (double(j) - xx) / sin(horizontalAngle) / cos(verticalAngle);
                
                if (t >= 10.0 || t <= -10.0) {
                    break;
                }else{
                    y = yy + sin(verticalAngle) * t;
                    z = zz + cos(verticalAngle) * cos(horizontalAngle) * t;
                    
                    int position[3];
                    if (positive) {
                        position[0] = j;
                    }else{
                        position[0] = j - 1;
                    }
                    
                    position[1] = floor(y);
                    position[2] = floor(z);
                    
                    int block = gchunk(position[0] + state.chunkoffset.x, position[1] + state.chunkoffset.y, position[2] + state.chunkoffset.z);
                    
                    if (block != 0) {
                        if (t < 0.0) {
                            t = -t;
                        }
                        if (t < distance) {
                            distance = t;
                            state.lookAtPos.x = position[0];
                            state.lookAtPos.y = position[1];
                            state.lookAtPos.z = position[2];
                            state.lookAtBlock = block;
                            if (positive) {
                                state.lookFace = GameState::RIGHT;
                            }else{
                                state.lookFace = GameState::LEFT;
                            }
                        }
                        break;
                    }
                    
                    if (positive) {
                        j++;
                    }else{
                        j--;
                    }
                }
                
            }
            
        }
        
        
        // test z axis
        j = floor(zz);
        positive = (cos(horizontalAngle) > 0.0);
        if (positive) {
            j++;
        }
        
        if (0) {
            //cos(horizontalAngle) > 0.9 || cos(horizontalAngle) < -0.9
        }else{
            while (true) {
                t = (double(j) - zz) / cos(horizontalAngle) / cos(verticalAngle);
                
                if (t >= 10.0 || t <= -10.0) {
                    break;
                }else{
                    x = xx + cos(verticalAngle) * sin(horizontalAngle) * t;
                    y = yy + sin(verticalAngle) * t;
                    
                    int position[3];
                    position[0] = floor(x);
                    position[1] = floor(y);
                    if (positive) {
                        position[2] = j;
                    }else{
                        position[2] = j - 1;
                    }
                    
                    int block = gchunk(position[0] + state.chunkoffset.x, position[1] + state.chunkoffset.y, position[2] + state.chunkoffset.z);
                    
                    if (block != 0) {
                        if (t < 0.0) {
                            t = -t;
                        }
                        if (t < distance) {
                            distance = t;
                            state.lookAtPos.x = position[0];
                            state.lookAtPos.y = position[1];
                            state.lookAtPos.z = position[2];
                            state.lookAtBlock = block;
                            if (positive) {
                                state.lookFace = GameState::BACK;
                            }else{
                                state.lookFace = GameState::FRONT;
                            }
                        }
                        break;
                    }
                    
                    if (positive) {
                        j++;
                    }else{
                        j--;
                    }
                }
                
            }
            
        }
        
        if (state.lookAtBlock == 0) {
            state.lookFace = GameState::NONE;
        }
    }
    
    void Handler::printPosition() {
        char buffer[64];
        sprintf(buffer, "(%d, %d, %d)", (int)state.position.x + state.chunkoffset.x, (int)state.position.y + state.chunkoffset.y, (int)state.position.z + state.chunkoffset.z);
        gterminal.println(std::string(buffer));
    }
    
    void Handler::printLookBlock() {
        const char * face;
        switch (state.lookFace) {
            case GameState::NONE:
                face = "NONE";
                break;
                
            case GameState::TOP:
                face = "TOP";
                break;
                
            case GameState::BOTTOM:
                face = "BOTTOM";
                break;
                
            case GameState::LEFT:
                face = "LEFT";
                break;
                
            case GameState::RIGHT:
                face = "RIGHT";
                break;
                
            case GameState::FRONT:
                face = "FRONT";
                break;
                
            case GameState::BACK:
                face = "BACK";
                break;
                
            default:
                break;
        }
        char buffer[128];
        sprintf(buffer, "((%d, %d, %d), (%d, %s))", state.lookAtPos.x + state.chunkoffset.x, state.lookAtPos.y + state.chunkoffset.y, state.lookAtPos.z + state.chunkoffset.z, state.lookAtBlock, face);
        gterminal.println(std::string(buffer));
    }

}
