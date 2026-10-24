#include "config.hpp"
#include "json.hpp"
#include <cmath>
Config config;
Setting setting;

namespace {
    bool powerOfTwo(int x) { return x > 0 && (x & (x-1)) == 0; }
    void require(bool valid, const char * field) {
        if (!valid) throw std::runtime_error(std::string("invalid configuration value: ") + field);
    }
}
bool importSetting(const std::string & path) {
    using namespace mineworld;
    try {
        auto document = readJson(path);
        const auto & c = jsonMember(document, "config");
        const auto & s = jsonMember(document, "setting");
        Config nextConfig{};
        Setting nextSetting{};
        nextConfig.blockFilePath = jsonString(c, "blockFilePath");
        const auto & fonts = jsonMember(c, "fontset");
        nextConfig.fontset.texture = jsonString(fonts, "texture");
        nextConfig.fontset.fontvsh = jsonString(fonts, "fontvsh");
        nextConfig.fontset.fontfsh = jsonString(fonts, "fontfsh");
        nextConfig.TEXTURE_SIZE = jsonInt(c, "TEXTURE_SIZE");
        nextConfig.ATLAS_SIZE_UPPER_BOUND = jsonInt(c, "ATLAS_SIZE_UPPER_BOUND");
        require(nextConfig.TEXTURE_SIZE >= 16 && powerOfTwo(nextConfig.TEXTURE_SIZE), "TEXTURE_SIZE");
        require(nextConfig.ATLAS_SIZE_UPPER_BOUND >= nextConfig.TEXTURE_SIZE &&
                powerOfTwo(nextConfig.ATLAS_SIZE_UPPER_BOUND), "ATLAS_SIZE_UPPER_BOUND");
        const auto & blocks = jsonMember(c, "blockshader");
        nextConfig.blockshader.fsh = jsonString(blocks, "fsh");
        nextConfig.blockshader.vsh = jsonString(blocks, "vsh");
        const auto & sky = jsonMember(c, "skybox");
        nextConfig.skybox.skyboxfsh = jsonString(sky, "skyboxfsh");
        nextConfig.skybox.skyboxvsh = jsonString(sky, "skyboxvsh");
        const char * faces[] = {"right","left","top","bottom","back","front"};
        for (int i = 0; i < 6; ++i) nextConfig.skybox.faces[i] = jsonString(sky, faces[i]);
        const auto & cross = jsonMember(c, "crosshair");
        nextConfig.crosshair.crossvsh = jsonString(cross, "crossvsh");
        nextConfig.crosshair.crossfsh = jsonString(cross, "crossfsh");
        nextConfig.crosshair.texturePath = jsonString(cross, "texturePath");
        nextConfig.VISIBLE_DISTANCE = jsonInt(c, "VISIBLE_DISTANCE");
        nextConfig.CHUNK_CACHE_DISTANCE = jsonInt(c, "CHUNK_CACHE_DISTANCE");
        require(nextConfig.VISIBLE_DISTANCE >= 0 && nextConfig.VISIBLE_DISTANCE <= 64, "VISIBLE_DISTANCE (0..64)");
        require(nextConfig.CHUNK_CACHE_DISTANCE >= nextConfig.VISIBLE_DISTANCE &&
                nextConfig.CHUNK_CACHE_DISTANCE <= 128, "CHUNK_CACHE_DISTANCE");
        nextSetting.SHOW_FPS = jsonBool(s, "SHOW_FPS");
        nextSetting.INVERT_Y_AXIS = jsonBool(s, "INVERT_Y_AXIS");
        nextSetting.VERTICAL_SYNC = jsonBool(s, "VERTICAL_SYNC");
        nextSetting.WINDOW_WIDTH = jsonInt(s, "WINDOW_WIDTH");
        nextSetting.WINDOW_HEIGHT = jsonInt(s, "WINDOW_HEIGHT");
        require(nextSetting.WINDOW_WIDTH > 0 && nextSetting.WINDOW_WIDTH <= 16384, "WINDOW_WIDTH");
        require(nextSetting.WINDOW_HEIGHT > 0 && nextSetting.WINDOW_HEIGHT <= 16384, "WINDOW_HEIGHT");
        nextSetting.CENTER_WIDTH = nextSetting.WINDOW_WIDTH / 2;
        nextSetting.CENTER_HEIGHT = nextSetting.WINDOW_HEIGHT / 2;
        nextSetting.walkSpeed = jsonFloat(s, "walkSpeed");
        nextSetting.mouseSpeed = jsonFloat(s, "mouseSpeed");
        require(std::isfinite(nextSetting.walkSpeed) && nextSetting.walkSpeed > 0, "walkSpeed");
        require(std::isfinite(nextSetting.mouseSpeed) && nextSetting.mouseSpeed > 0, "mouseSpeed");
        nextSetting.username = jsonString(s, "username");
        nextSetting.FOV = jsonFloat(s, "FOV");
        require(std::isfinite(nextSetting.FOV) && nextSetting.FOV > 0 && nextSetting.FOV < 180, "FOV");
        const auto & p = jsonMember(s, "initialPos");
        nextSetting.initialPos = {jsonInt(p,"x"), jsonInt(p,"y"), jsonInt(p,"z")};
        config = std::move(nextConfig);
        setting = std::move(nextSetting);
        return true;
    } catch (const std::exception & error) {
        std::cerr << "[config] " << error.what() << std::endl;
        return false;
    }
}
