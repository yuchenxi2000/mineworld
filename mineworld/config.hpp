#ifndef config_hpp
#define config_hpp

#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>
#include "util.hpp"
/*
 * user config and settings.
 * call importSetting() when program starts.
 */
struct Config {
    std::string blockFilePath;
    struct {
        std::string texture, fontvsh, fontfsh;
    } fontset;
    int TEXTURE_SIZE;
    int VISIBLE_DISTANCE;
    int CHUNK_CACHE_DISTANCE;
    int ATLAS_SIZE_UPPER_BOUND;
    
    struct {
        std::string fsh, vsh;
    } blockshader;
    struct {
        std::string shadowfsh, shadowvsh;
        int ShadowMapSize;
    } shadowmap;
    struct {
        std::string skyboxfsh, skyboxvsh;
        std::string faces[6];
    } skybox;
    struct {
        std::string crossvsh, crossfsh;
        std::string texturePath;
    } crosshair;
};

struct Setting {
    float FOV = 45.0f;
    bool INVERT_Y_AXIS = false;
    bool VERTICAL_SYNC = true;
    
    int KEY_FORWARD = GLFW_KEY_W;
    int KEY_BACKWARD = GLFW_KEY_S;
    int KEY_LEFT = GLFW_KEY_A;
    int KEY_RIGHT = GLFW_KEY_D;
    int KEY_UP = GLFW_KEY_SPACE;
    int KEY_DOWN = GLFW_KEY_LEFT_SHIFT;
    
    int KEY_ESC = GLFW_KEY_ESCAPE;
    int KEY_TYPE = GLFW_KEY_T;
    
    int WINDOW_WIDTH = 800;
    int WINDOW_HEIGHT = 600;
    
    int CENTER_WIDTH = 400;
    int CENTER_HEIGHT = 300;
    
    int FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT;
    
    float mouseSpeed = 0.005f;
    float walkSpeed = 3.0f;
    
    bool SHOW_FPS = 0;
    
    std::string username;
    
    mineworld2::ivec3 initialPos;
};

extern Config config;
extern Setting setting;

/*
 * read from "config.json"
 */
bool importSetting();

#endif /* config_hpp */
