#include "mineworld.hpp"
#include "terminal.hpp"
#include "worker.hpp"
#include "texture.hpp"
#include "world.hpp"
#include "block.hpp"
#include <fstream>
#ifdef _WIN32
#include <direct.h>
#else
#include <unistd.h>
#endif
#include <memory>
#include <stdexcept>

namespace {
    // Keep shutdown in one place, while the GL context and block definitions live.
    class Game {
        GLFWwindow * window = nullptr;
        std::unique_ptr<mineworld::Player> player;
    public:
        ~Game() {
            worker::workerStop();
            mineworld::gchunk.clear();
            mineworld::handler.player = nullptr;
            mineworld::handler.window = nullptr;
            player.reset();
            mineworld::gterminal.release(); mineworld::gboard.release();
            mineworld::gcrosshairshader.release(); mineworld::gblockshader.release();
            mineworld::gskyboxshader.release(); mineworld::gtexturemanager.release();
            mineworld::gblockregister.blockTable.clear();
            mineworld::gblockregister.map_name_id.clear();
            if (window) glfwDestroyWindow(window);
            glfwTerminate();
        }
        void run() {
            using namespace mineworld;
            if (!importSetting()) throw std::runtime_error("failed to import settings");
            window = glfwStart();
            if (!window) throw std::runtime_error("failed to create OpenGL window");
            gblockregister.loadBlock();
            gworldgenerator.init();
            gskyboxshader.init(); gblockshader.init(); gcrosshairshader.init();
            block_loc_t start(setting.initialPos);
            entity_pos_t position(start.chunkpos, glm::vec3(start.offset));
            position += glm::vec3(0.5f,3,0.5f);
            player = std::make_unique<Player>(setting.username,position);
            handler.init(window,player.get());
            gterminal.init(); gboard.init();
            installWindowCallbacks(window);
            worker::workerStart(4);
            while (!glfwWindowShouldClose(window)) {
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                glfwPollEvents();
                handler.handle_input();
                const auto & view = handler.ViewMat();
                const auto & projection = handler.ProjMat();
                gblockshader.use(); gblockshader.setuniform(view,projection); gchunk.load();
                gskyboxshader.use(); gskyboxshader.setuniform(view,projection); gskyboxshader.drawSkyBox();
                if (handler.playstate == TYPING) { gterminal.flush(); gterminal.show(); }
                else { gcrosshairshader.drawCrosshair(); gboard.flush(); gboard.show(); }
                glfwSwapBuffers(window);
            }
        }
    };
    void locateResources(const char * executable) {
        if (std::ifstream("config.json")) return;
        const std::string path(executable);
        const auto slash = path.find_last_of("/\\");
        const std::string directory = slash == std::string::npos ? "." : path.substr(0,slash);
        for (const auto & candidate : {directory+"/res", directory+"/../res", directory+"/../../res", directory+"/../Resources"}) {
            if (!std::ifstream(candidate+"/config.json")) continue;
#ifdef _WIN32
            const int changed = _chdir(candidate.c_str());
#else
            const int changed = chdir(candidate.c_str());
#endif
            if (changed != 0) throw std::runtime_error("cannot enter resource directory: " + candidate);
            return;
        }
    }
}
int main(int, const char * argv[]) {
    try {
        locateResources(argv[0]);
        Game game;
        game.run();
        return 0;
    } catch (const std::exception & error) {
        std::cerr << "[main] " << error.what() << std::endl;
        return 1;
    }
}
