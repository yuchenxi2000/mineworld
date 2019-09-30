#include "block.hpp"

namespace mineworld {
    BlockRegister gblockregister;
    
    void Block::leftClick(const block_loc_t & pos, const hit_pos_t & hitpos, int holdblock) {
        if (hitpos.face != NONE)
            gchunk.blockUpdate(pos, 0);
    }
    
    void Block::rightClick(const block_loc_t & pos, const hit_pos_t & hitpos, int holdblock) {
        block_loc_t position(pos);
        switch (hitpos.face) {
            case TOP:
                position += {0, 1, 0};
                break;
                
            case BOTTOM:
                position += {0, -1, 0};
                break;
                
            case LEFT:
                position += {1, 0, 0};
                break;
                
            case RIGHT:
                position += {-1, 0, 0};
                break;
                
            case FRONT:
                position += {0, 0, 1};
                break;
                
            case BACK:
                position += {0, 0, -1};
                break;
                
            case NONE:
                return;
                
            default:
                break;
        }
        if (gchunk(position) == 0)
            gchunk.blockUpdate(position, ID);
    }
    
//    void Block::leftClick(const glm::ivec3 & pos, Face face) {
//        if (face != NONE) {
//            gchunk.blockUpdate(pos, 0);
//        }
//    }
//    void Block::rightClick(const glm::ivec3 & pos, Face face) {
//        glm::ivec3 position(pos);
//        switch (face) {
//            case TOP:
//                position.y++;
//                break;
//
//            case BOTTOM:
//                position.y--;
//                break;
//
//            case LEFT:
//                position.x++;
//                break;
//
//            case RIGHT:
//                position.x--;
//                break;
//
//            case FRONT:
//                position.z++;
//                break;
//
//            case BACK:
//                position.z--;
//                break;
//
//            case NONE:
//                return;
//
//            default:
//                break;
//        }
//        gchunk.blockUpdate(position, ID);
//    }
    
    bool Block::hit(const glm::vec3 & position, const glm::vec3 & direction, hit_pos_t & hitpos) {
        hit_pos_t lhp;
        hitpos = hit_none;
        for (auto & box : vhitbox) {
            lhp = box.hit(position, direction);
            if (hitpos.distance > lhp.distance) {
                hitpos = lhp;
            }
        }
        return hitpos.distance < INFINITY;
    }
    
    Cube::Cube(rapidjson::Value & blockinfo) : Block() {
        
        vhitbox.push_back(Hitbox({0, 0, 0}, {1, 1, 1}, 0));
        
        isComplete = true;
        name = blockinfo["name"].GetString();

        rapidjson::Value & textureinfo = blockinfo["texture"];
        std::string path;
        texture_loc_t loc;
        
        path = textureinfo["upper"].GetString();
        loc = gtexturemanager.loadTexture(path);
        model.push_back(rectY({0, 1, 1}, {1, 1, 0}, {0, 0}, {1, 1}, loc));
        
        path = textureinfo["bottom"].GetString();
        loc = gtexturemanager.loadTexture(path);
        model.push_back(rectY({1, 0, 1}, {0, 0, 0}, {0, 0}, {1, 1}, loc));
        
        path = textureinfo["left"].GetString();
        loc = gtexturemanager.loadTexture(path);
        model.push_back(rectX({1, 0, 1}, {1, 1, 0}, {0, 0}, {1, 1}, loc));
        
        path = textureinfo["right"].GetString();
        loc = gtexturemanager.loadTexture(path);
        model.push_back(rectX({0, 0, 0}, {0, 1, 1}, {0, 0}, {1, 1}, loc));
        
        path = textureinfo["front"].GetString();
        loc = gtexturemanager.loadTexture(path);
        model.push_back(rectZ({0, 0, 1}, {1, 1, 1}, {0, 0}, {1, 1}, loc));
        
        path = textureinfo["back"].GetString();
        loc = gtexturemanager.loadTexture(path);
        model.push_back(rectZ({1, 0, 0}, {0, 1, 0}, {0, 0}, {1, 1}, loc));
        
    }
    
    void Cube::addVertex(std::vector<rect> * vertexarray, Cell * cell, glm::ivec3 & pos) {
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
    
    Slab::Slab(rapidjson::Value & blockinfo) : Block() {
        
        vhitbox.push_back(Hitbox({0, 0, 0}, {1, 0.5, 1}, 0));
        
        isComplete = false;
        name = blockinfo["name"].GetString();
        
        rapidjson::Value & textureinfo = blockinfo["texture"];
        
        std::string path;
        texture_loc_t loc;
        
        path = textureinfo["upper"].GetString();
        loc = gtexturemanager.loadTexture(path);
        model.push_back(rectY({0, 0.5, 1}, {1, 0.5, 0}, {0, 0}, {1, 1}, loc));
        
        path = textureinfo["bottom"].GetString();
        loc = gtexturemanager.loadTexture(path);
        model.push_back(rectY({1, 0, 1}, {0, 0, 0}, {0, 0}, {1, 1}, loc));
        
        path = textureinfo["left"].GetString();
        loc = gtexturemanager.loadTexture(path);
        model.push_back(rectX({1, 0, 1}, {1, 0.5, 0}, {0, 0}, {1, 0.5}, loc));
        
        path = textureinfo["right"].GetString();
        loc = gtexturemanager.loadTexture(path);
        model.push_back(rectX({0, 0, 0}, {0, 0.5, 1}, {0, 0}, {1, 0.5}, loc));
        
        path = textureinfo["front"].GetString();
        loc = gtexturemanager.loadTexture(path);
        model.push_back(rectZ({0, 0, 1}, {1, 0.5, 1}, {0, 0}, {1, 0.5}, loc));
        
        path = textureinfo["back"].GetString();
        loc = gtexturemanager.loadTexture(path);
        model.push_back(rectZ({1, 0, 0}, {0, 0.5, 0}, {0, 0}, {1, 0.5}, loc));
    }
    
    Stairs::Stairs(rapidjson::Value & blockinfo) {
        vhitbox.push_back(Hitbox({0, 0, 0}, {1, 1, 0.5}, 0));
        vhitbox.push_back(Hitbox({0, 0, 0.5}, {1, 0.5, 1}, 1));
        
        isComplete = false;
        name = blockinfo["name"].GetString();
        
        rapidjson::Value & textureinfo = blockinfo["texture"];
        
        std::string path;
        texture_loc_t loc;
        
        path = textureinfo["upper"].GetString();
        loc = gtexturemanager.loadTexture(path);
        model.push_back(rectY({0, 0.5, 1}, {1, 0.5, 0.5}, {0, 0}, {1, 0.5}, loc));
        model.push_back(rectY({0, 1, 0.5}, {1, 1, 0}, {0, 0.5}, {1, 1}, loc));
        
        path = textureinfo["bottom"].GetString();
        loc = gtexturemanager.loadTexture(path);
        model.push_back(rectY({1, 0, 1}, {0, 0, 0}, {0, 0}, {1, 1}, loc));
        
        path = textureinfo["left"].GetString();
        loc = gtexturemanager.loadTexture(path);
        model.push_back(rectX({1, 0, 1}, {1, 1, 0}, {0, 0}, {1, 1}, loc));
        
        path = textureinfo["right"].GetString();
        loc = gtexturemanager.loadTexture(path);
        model.push_back(rectX({0, 0, 0}, {0, 1, 1}, {0, 0}, {1, 1}, loc));
        
        path = textureinfo["front"].GetString();
        loc = gtexturemanager.loadTexture(path);
        model.push_back(rectZ({0, 0, 1}, {1, 0.5, 1}, {0, 0}, {1, 0.5}, loc));
        model.push_back(rectZ({0, 0.5, 0.5}, {1, 1, 0.5}, {0, 0.5}, {1, 1}, loc));
        
        path = textureinfo["back"].GetString();
        loc = gtexturemanager.loadTexture(path);
        model.push_back(rectZ({1, 0, 0}, {0, 1, 0}, {0, 0}, {1, 1}, loc));
    }
    
    Rail::Rail(rapidjson::Value & blockinfo) {
        vhitbox.push_back(Hitbox({0, 0, 0}, {1, 0.125, 1}, 0));
        
        isComplete = false;
        name = blockinfo["name"].GetString();
        
        rapidjson::Value & textureinfo = blockinfo["texture"];
        
        std::string path;
        texture_loc_t loc;
        
        path = textureinfo["upper"].GetString();
        loc = gtexturemanager.loadTexture(path);
        model.push_back(rectY({0, 0.0625, 1}, {1, 0.0625, 0}, {0, 0}, {1, 1}, loc));
        model.push_back(rectY({1, 0.0625, 1}, {0, 0.0625, 0}, {1, 0}, {0, 1}, loc));
    }
    
    Grass::Grass(rapidjson::Value & blockinfo) {
        vhitbox.push_back(Hitbox({0, 0, 0}, {1, 0.7, 1}, 0));
        
        isComplete = false;
        name = blockinfo["name"].GetString();
        
        rapidjson::Value & textureinfo = blockinfo["texture"];
        
        std::string path;
        texture_loc_t loc;
        
        path = textureinfo["cross1"].GetString();
        loc = gtexturemanager.loadTexture(path);
        model.push_back(rectP({0, 0, 1}, {1, 0, 0}, {1, 1, 0}, {0, 1, 1}, {0, 0}, {1, 1}, loc));
        model.push_back(rectP({1, 0, 0}, {0, 0, 1}, {0, 1, 1}, {1, 1, 0}, {0, 0}, {1, 1}, loc));
        
        path = textureinfo["cross2"].GetString();
        loc = gtexturemanager.loadTexture(path);
        model.push_back(rectP({1, 0, 1}, {0, 0, 0}, {0, 1, 0}, {1, 1, 1}, {0, 0}, {1, 1}, loc));
        model.push_back(rectP({0, 0, 0}, {1, 0, 1}, {1, 1, 1}, {0, 1, 0}, {0, 0}, {1, 1}, loc));
    }
    
    void BlockRegister::loadBlock() {
        addBlock(new Air()); // air block
        
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
                    addBlock(new Cube(block));
                }else if (renderType == "glass") {
                    addBlock(new Glass(block));
                }else if (renderType == "slab") {
                    addBlock(new Slab(block));
                }else if (renderType == "stairs") {
                    addBlock(new Stairs(block));
                }else if (renderType == "rail") {
                    addBlock(new Rail(block));
                }else if (renderType == "grass") {
                    addBlock(new Grass(block));
                }else {
                    std::cerr << std::string("[block] unknown render type. skip block ") + blockname << std::endl;
                }
            }
            
        }
        gtexturemanager.finishLoadTexture();
    }
    
    void BlockRegister::listBlock() {
        for (auto & p : map_name_id) {
            gterminal.println(p.first);
        }
    }
    
    bool blockComplete(int block) {
        return gblockregister.getBlockbyID(block)->isComplete;
    }
    
}
