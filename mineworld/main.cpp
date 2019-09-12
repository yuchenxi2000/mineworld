#include "mineworld.hpp"

int main(int argc, const char * argv[]) {
    // create window & context, make context current
    GLFWwindow * window = mineworld2::glfwStart();
    if (window == 0) {
        std::cerr << "[main] failed to open window." << std::endl;
        return -1;
    }
    
    // import setting from "config.json"
    if (!importSetting()) {
        std::cerr << "[main] failed to import settings." << std::endl;
        return -1;
    }
    // resize window according to user settings.
    glfwSetWindowSize(window, setting.WINDOW_WIDTH, setting.WINDOW_HEIGHT);
    glfwGetFramebufferSize(window, &setting.FRAME_BUFFER_WIDTH, &setting.FRAME_BUFFER_HEIGHT);
    
    // load blocks from block file specified in "config.json"
    mineworld2::gblockregister.loadBlock();
    mineworld2::gworldgenerator.init();
    
    // init shaders.
    // init skybox shader before block shader.
    mineworld2::gskyboxshader.init();
    mineworld2::gblockshader.init();
    mineworld2::gcrosshairshader.init();
    
    // init handler
    mineworld2::handler.init(window);
    
    mineworld2::gterminal.init();
    mineworld2::gboard.init();
    
    worker::workerStart(4);
    
    // start game loop
    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glfwPollEvents();
        mineworld2::handler.handle_input();
        
        glm::mat4 & view = mineworld2::handler.ViewMat();
        glm::mat4 & proj = mineworld2::handler.ProjMat();
//        glm::mat4 skyboxview = glm::mat4(glm::mat3(view));
        
        mineworld2::gblockshader.use();
        mineworld2::gblockshader.setuniform(view, proj);
        mineworld2::gchunk.load();
        
        mineworld2::gskyboxshader.use();
        mineworld2::gskyboxshader.setuniform(view, proj);
        mineworld2::gskyboxshader.drawSkyBox();
        
        if (mineworld2::handler.state.PlayingState == mineworld2::GameState::TYPING) {
            mineworld2::gterminal.flush();
            mineworld2::gterminal.show();
        }else {
            mineworld2::gcrosshairshader.drawCrosshair();
            mineworld2::gboard.flush();
            mineworld2::gboard.show();
        }
        
        glfwSwapBuffers(window);
    }
    
    glfwTerminate();
    std::cout << "[main] game terminated" << std::endl;
    return 0;
}
