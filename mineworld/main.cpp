#include "mineworld.hpp"

int main(int argc, const char * argv[]) {
    // create window & context, make context current
    GLFWwindow * window = mineworld::glfwStart();
    if (window == 0) {
        std::cerr << "[main] failed to open window." << std::endl;
        return -1;
    }
    
    // import setting from "config.json"
    if (!importSetting()) {
        std::cerr << "[main] failed to import settings." << std::endl;
        return -1;
    }
    
    // vertical sync
    if (setting.VERTICAL_SYNC)
        glfwSwapInterval(1);
    
    // resize window according to user settings.
    glfwSetWindowSize(window, setting.WINDOW_WIDTH, setting.WINDOW_HEIGHT);
    glfwGetFramebufferSize(window, &setting.FRAME_BUFFER_WIDTH, &setting.FRAME_BUFFER_HEIGHT);
    
    // load blocks from block file specified in "config.json"
    mineworld::gblockregister.loadBlock();
    mineworld::gworldgenerator.init();
    
    // init shaders.
    // init skybox shader before block shader.
    mineworld::gskyboxshader.init();
    mineworld::gblockshader.init();
    mineworld::gcrosshairshader.init();
    
    // calculate initial position
    mineworld::block_loc_t bl = setting.initialPos;
    mineworld::entity_pos_t initialpos;
    initialpos.chunkpos = bl.chunkpos;
    initialpos.offset = glm::vec3(bl.offset) + glm::vec3(0.5, 3, 0.5);
    // player "me" joined the game.
    mineworld::Player * me = new mineworld::Player(setting.username, initialpos);
    // init handler
    mineworld::handler.init(window, me);
    
    mineworld::gterminal.init();
    mineworld::gboard.init();
    
    worker::workerStart(4);
    
    // start game loop
    while (!glfwWindowShouldClose(window)) {
        glClear(GL_DEPTH_BUFFER_BIT);
        glfwPollEvents();
        mineworld::handler.handle_input();
        
        glm::mat4 & view = mineworld::handler.ViewMat();
        glm::mat4 & proj = mineworld::handler.ProjMat();
//        glm::mat4 skyboxview = glm::mat4(glm::mat3(view));
        
        mineworld::gblockshader.use();
        mineworld::gblockshader.setuniform(view, proj);
        mineworld::gchunk.load();
        
        mineworld::gskyboxshader.use();
        mineworld::gskyboxshader.setuniform(view, proj);
        mineworld::gskyboxshader.drawSkyBox();
        
        if (mineworld::handler.playstate == mineworld::TYPING) {
            mineworld::gterminal.flush();
            mineworld::gterminal.show();
        }else {
            mineworld::gcrosshairshader.drawCrosshair();
            mineworld::gboard.flush();
            mineworld::gboard.show();
        }
        
        glfwSwapBuffers(window);
    }
    
    glfwTerminate();
    std::cout << "[main] game terminated" << std::endl;
    return 0;
}
