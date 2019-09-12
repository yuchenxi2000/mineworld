#include "config.hpp"
#include <fstream>
#include <rapidjson/document.h>
#include <rapidjson/filereadstream.h>
Config config;
Setting setting;

bool is2pow(int x) {
    return (x & (x - 1)) == 0;
}

bool importSetting() {
    FILE * fp = fopen("config.json", "r");
    if (fp == 0) {
        std::cerr << "[config] config file not found" << std::endl;
        return false;
    }
    char readBuffer[2048];
    rapidjson::FileReadStream fs(fp, readBuffer, sizeof(readBuffer));
    rapidjson::Document configFile;
    configFile.ParseStream(fs);
    
    try {
        rapidjson::Value & configValue = configFile["config"];
        config.blockFilePath = configValue["blockFilePath"].GetString();
        config.fontset.texture = configValue["fontset"]["texture"].GetString();
        config.fontset.fontvsh = configValue["fontset"]["fontvsh"].GetString();
        config.fontset.fontfsh = configValue["fontset"]["fontfsh"].GetString();
        config.TEXTURE_SIZE = configValue["TEXTURE_SIZE"].GetInt();
        if (config.TEXTURE_SIZE < 16 || !is2pow(config.TEXTURE_SIZE)) {
            config.TEXTURE_SIZE = 16;
            std::cerr << "[config] invalid value : texture size, use default 16" << std::endl;
        }
        std::cout << "[config] texture size " << config.TEXTURE_SIZE << std::endl;
        
        config.ATLAS_SIZE_UPPER_BOUND = configValue["ATLAS_SIZE_UPPER_BOUND"].GetInt();
        if (config.ATLAS_SIZE_UPPER_BOUND < config.TEXTURE_SIZE || \
            !is2pow(config.ATLAS_SIZE_UPPER_BOUND)) {
            config.ATLAS_SIZE_UPPER_BOUND = 1024;
            std::cerr << "[config] invalid value : atlas size upper bound, use default 1024" << std::endl;
        }

        config.blockshader.fsh = configValue["blockshader"]["fsh"].GetString();
        config.blockshader.vsh = configValue["blockshader"]["vsh"].GetString();
        config.shadowmap.shadowfsh = configValue["shadowmap"]["shadowMapfsh"].GetString();
        config.shadowmap.shadowvsh = configValue["shadowmap"]["shadowMapvsh"].GetString();
        config.shadowmap.ShadowMapSize = configValue["shadowmap"]["ShadowMapSize"].GetInt();
        config.skybox.skyboxfsh = configValue["skybox"]["skyboxfsh"].GetString();
        config.skybox.skyboxvsh = configValue["skybox"]["skyboxvsh"].GetString();
        config.skybox.faces[0] = configValue["skybox"]["right"].GetString();
        config.skybox.faces[1] = configValue["skybox"]["left"].GetString();
        config.skybox.faces[2] = configValue["skybox"]["top"].GetString();
        config.skybox.faces[3] = configValue["skybox"]["bottom"].GetString();
        config.skybox.faces[4] = configValue["skybox"]["back"].GetString();
        config.skybox.faces[5] = configValue["skybox"]["front"].GetString();
        config.crosshair.crossvsh = configValue["crosshair"]["crossvsh"].GetString();
        config.crosshair.crossfsh = configValue["crosshair"]["crossfsh"].GetString();
        config.crosshair.texturePath = configValue["crosshair"]["texturePath"].GetString();
        
        config.VISIBLE_DISTANCE = configValue["VISIBLE_DISTANCE"].GetInt();
        config.CHUNK_CACHE_DISTANCE = configValue["CHUNK_CACHE_DISTANCE"].GetInt();
        
        rapidjson::Value & settingValue = configFile["setting"];
        setting.SHOW_FPS = settingValue["SHOW_FPS"].GetBool();
        setting.INVERT_Y_AXIS = settingValue["INVERT_Y_AXIS"].GetBool();
        setting.VERTICAL_SYNC = settingValue["VERTICAL_SYNC"].GetBool();
        
        //screen width
        setting.WINDOW_WIDTH = settingValue["WINDOW_WIDTH"].GetInt();
        setting.CENTER_WIDTH = setting.WINDOW_WIDTH / 2;
        
        //screen height
        setting.WINDOW_HEIGHT = settingValue["WINDOW_HEIGHT"].GetInt();
        setting.CENTER_HEIGHT = setting.WINDOW_HEIGHT / 2;
        
        setting.walkSpeed = settingValue["walkSpeed"].GetFloat();
        setting.mouseSpeed = settingValue["mouseSpeed"].GetFloat();
        
        setting.username = settingValue["username"].GetString();
        
        setting.FOV = settingValue["FOV"].GetFloat();
        
        setting.initialPos.x = settingValue["initialPos"]["x"].GetInt();
        setting.initialPos.y = settingValue["initialPos"]["y"].GetInt();
        setting.initialPos.z = settingValue["initialPos"]["z"].GetInt();
        
        fclose(fp);
    } catch (...) {
        fclose(fp);
        std::cerr << "[config] not a correct format" << std::endl;
        return false;
    }
    
    return true;
}
