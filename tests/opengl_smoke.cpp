#include "mineworld.hpp"
#include "block.hpp"
#include "terminal.hpp"
#include "texture.hpp"
#include "worker.hpp"
#include "world.hpp"
#include <chrono>
#include <stdexcept>
#include <thread>

using namespace mineworld;
namespace {
    void require(bool condition, const char * message) {
        if (!condition) throw std::runtime_error(message);
    }
    void checkGl() {
        GLenum error = glGetError();
        if (error != GL_NO_ERROR) throw std::runtime_error("OpenGL error: " + std::to_string(error));
    }
    void release() {
        worker::workerStop();
        gchunk.clear();
        handler.player = nullptr; handler.window = nullptr;
        gterminal.release(); gboard.release();
        gcrosshairshader.release(); gblockshader.release(); gskyboxshader.release();
        gtexturemanager.release();
        gblockregister.blockTable.clear(); gblockregister.map_name_id.clear();
    }
}
int main() {
    GLFWwindow * window = nullptr;
    try {
        require(importSetting(),"configuration failed");
        require(glfwInit() == GLFW_TRUE,"GLFW unavailable");
        glfwWindowHint(GLFW_VISIBLE,GLFW_FALSE);
        setting.VERTICAL_SYNC = false;
        window = glfwStart();
        require(window != nullptr,"OpenGL 3.3 unavailable");
        gblockregister.loadBlock(); gworldgenerator.init();
        gskyboxshader.init(); gblockshader.init(); gcrosshairshader.init();
        Player player("smoke",entity_pos_t({0,0,0},{0.5f,3,0.5f}));
        handler.init(window,&player);
        gterminal.init(); gboard.init();
        installWindowCallbacks(window);
        worker::workerStart(4);
        checkGl();
        auto render = [&] {
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            const glm::mat4 view = glm::lookAt(glm::vec3(0),glm::vec3(0,0,1),glm::vec3(0,1,0));
            gblockshader.use(); gblockshader.setuniform(view,handler.ProjMat()); gchunk.load();
            gskyboxshader.use(); gskyboxshader.setuniform(view,handler.ProjMat()); gskyboxshader.drawSkyBox();
            gcrosshairshader.drawCrosshair(); gboard.flush(); gboard.show();
            gterminal.flush(); gterminal.show();
            glfwSwapBuffers(window); glfwPollEvents();
            checkGl();
        };
        auto settle = [&] {
            auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(10);
            do {
                require(std::chrono::steady_clock::now() < deadline,"mesh timeout");
                render();
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            } while (gchunk.hasPendingMeshes());
        };
        settle();
        require(gterminal.executeLine("set 8 24 8 1"),"set command failed");
        settle();
        require(gchunk.findCell({0,16,0})->glbuffer.size() == 6,"single cube mesh failed");
        require(gterminal.executeLine("set 8 24 8 0"),"delete command failed");
        settle();
        require(gchunk.findCell({0,16,0})->glbuffer.size() == 0,"ghost GPU mesh");
        glfwSetWindowSize(window,640,480); render();
        // Restore pending uploads after a resize, and exercise normal terminal exit.
        render();
        require(gterminal.executeLine("exit") && glfwWindowShouldClose(window),"terminal exit bypassed game shutdown");
        release();
        checkGl();
        glfwDestroyWindow(window); window = nullptr;
        glfwTerminate();
        std::cout << "OpenGL initialization/render/edit/resize/shutdown passed\n";
        return 0;
    } catch (const std::exception & error) {
        release();
        if (window) glfwDestroyWindow(window);
        glfwTerminate();
        std::cerr << "FAIL: " << error.what() << '\n';
        return 1;
    }
}
