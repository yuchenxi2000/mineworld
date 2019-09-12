#include "block.hpp"

namespace mineworld2 {
    BlockRegister gblockregister;
    
    Cube::Cube(rapidjson::Value & blockinfo) : Block() {
        static const char * facesname[6] = {"upper", "bottom", "left", "right", "front", "back"};
        static const float vert[6][32] = {
            {
                // upper
                0, 1, 0, 0, 0, 0, 1, 0,
                0, 1, 1, 0, 1, 0, 1, 0,
                1, 1, 1, 1, 1, 0, 1, 0,
                1, 1, 0, 1, 0, 0, 1, 0,
            },
            {
                // bottom
                0, 0, 0, 0, 1, 0, -1, 0,
                1, 0, 0, 1, 1, 0, -1, 0,
                1, 0, 1, 1, 0, 0, -1, 0,
                0, 0, 1, 0, 0, 0, -1, 0,
            },
            {
                // left
                0, 1, 0, 0, 0, -1, 0, 0,
                0, 0, 0, 0, 1, -1, 0, 0,
                0, 0, 1, 1, 1, -1, 0, 0,
                0, 1, 1, 1, 0, -1, 0, 0,
            },
            {
                // right
                1, 0, 1, 0, 1, 1, 0, 0,
                1, 0, 0, 1, 1, 1, 0, 0,
                1, 1, 0, 1, 0, 1, 0, 0,
                1, 1, 1, 0, 0, 1, 0, 0,
            },
            {
                // front
                0, 1, 1, 0, 0, 0, 0, 1,
                0, 0, 1, 0, 1, 0, 0, 1,
                1, 0, 1, 1, 1, 0, 0, 1,
                1, 1, 1, 1, 0, 0, 0, 1,
            },
            {
                // back
                1, 0, 0, 0, 1, 0, 0, -1,
                0, 0, 0, 1, 1, 0, 0, -1,
                0, 1, 0, 1, 0, 0, 0, -1,
                1, 1, 0, 0, 0, 0, 0, -1,
            },
        };
        
        isComplete = true;
        name = blockinfo["name"].GetString();
        
        for (int i = 0; i < 6; ++i) {
            model.push_back(rect(vert[i]));
        }
        rapidjson::Value & textureinfo = blockinfo["texture"];
        for (int i = 0; i < 6; ++i) {
            std::string path = textureinfo[facesname[i]].GetString();
            Textureloc loc = gtexturemanager.loadTexture(path);
            for (int k = 0; k < 4; ++k) {
                model[i].v[k].uv[0] *= config.TEXTURE_SIZE;
                model[i].v[k].uv[0] += loc.x;
                model[i].v[k].uv[0] /= gtexturemanager.MAX_TEXTURE_SIZE;
                model[i].v[k].uv[1] *= config.TEXTURE_SIZE;
                model[i].v[k].uv[1] += loc.y;
                model[i].v[k].uv[1] /= gtexturemanager.MAX_TEXTURE_SIZE;
            }
        }
    }
    
    void Cube::addVertex(std::vector<rect> * vertexarray, Cell * cell, ivec3 & pos) {
        int block;
        
        block = cell->upper(pos);
        if (!blockComplete(block)) {
            vertexarray->push_back(shift(model[0], pos));
        }
        block = cell->bottom(pos);
        if (!blockComplete(block)) {
            vertexarray->push_back(shift(model[1], pos));
        }
        block = cell->left(pos);
        if (!blockComplete(block)) {
            vertexarray->push_back(shift(model[2], pos));
        }
        block = cell->right(pos);
        if (!blockComplete(block)) {
            vertexarray->push_back(shift(model[3], pos));
        }
        block = cell->front(pos);
        if (!blockComplete(block)) {
            vertexarray->push_back(shift(model[4], pos));
        }
        block = cell->back(pos);
        if (!blockComplete(block)) {
            vertexarray->push_back(shift(model[5], pos));
        }
        
    }
    
    void BlockRegister::loadBlock() {
        blockTable.push_back(new Air()); // air block
        map_name_id[std::string("air")] = 0;
        
        FILE * fp = fopen(config.blockFilePath.c_str(), "r");
        if (!fp) {
            std::cerr << "[block] failed to open block file" << std::endl;
            return;
        }
        char readBuffer[2048];
        rapidjson::FileReadStream fs(fp, readBuffer, sizeof(readBuffer));
        rapidjson::Document blockFile;
        blockFile.ParseStream(fs);
        
        gtexturemanager.init();
        for (auto & package : blockFile.GetArray()) {
            std::string packageName = package["package"].GetString();
            std::cout << std::string("[block] package name ") + packageName << std::endl;
            
            rapidjson::Value & blockinfo = package["block"];
            for (auto & block : blockinfo.GetArray()) {
                std::string renderType = block["type"].GetString();
                std::string blockname = block["name"].GetString();
                if (renderType == "cube") {
                    map_name_id[blockname] = blockTable.size();
                    blockTable.push_back(new Cube(block));
                }else if (renderType == "glass") {
                    map_name_id[blockname] = blockTable.size();
                    blockTable.push_back(new Glass(block));
                }else {
                    std::cerr << std::string("[block] unknown render type. skip block ") + blockname << std::endl;
                }
            }
            
        }
        gtexturemanager.finishLoadTexture();
    }
    
    bool blockComplete(int block) {
        return gblockregister.blockTable[block]->isComplete;
    }
    
}
