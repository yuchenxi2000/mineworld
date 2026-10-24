#include "block.hpp"
#include "cell.hpp"
#include "terminal.hpp"
#include "json.hpp"

namespace mineworld {
    BlockRegister gblockregister;
    
    void Block::leftClick(const block_loc_t & pos, const hit_pos_t & hitpos, int) {
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
            gchunk.blockUpdate(position, holdblock);
    }
    
    bool Block::hit(const glm::vec3 & position, const glm::vec3 & direction, hit_pos_t & hitpos) const {
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
    
    void Cube::addVertex(std::vector<rect> * vertexarray, const MeshSnapshot * cell, const glm::ivec3 & pos) const {
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
        auto blockFile = readJson(config.blockFilePath);
        if (!blockFile.IsArray()) throw std::runtime_error("block file must contain an array");
        // Validate before constructors access JSON or allocate GPU resources.
        std::map<std::string, bool> names{{"air", true}};
        for (auto & package : blockFile.GetArray()) {
            jsonString(package, "package");
            const auto & blocks = jsonMember(package, "block");
            if (!blocks.IsArray()) throw std::runtime_error("block field must be an array");
            for (const auto & block : blocks.GetArray()) {
                auto name = jsonString(block, "name");
                auto type = jsonString(block, "type");
                if (name.empty() || !names.emplace(name, true).second)
                    throw std::runtime_error("empty or duplicate block name: " + name);
                const auto & texture = jsonMember(block, "texture");
                if (type == "grass") {
                    jsonString(texture, "cross1"); jsonString(texture, "cross2");
                } else if (type == "rail") {
                    jsonString(texture, "upper");
                } else if (type == "cube" || type == "glass" || type == "slab" || type == "stairs") {
                    for (const char * face : {"upper", "bottom", "left", "right", "front", "back"}) jsonString(texture, face);
                } else throw std::runtime_error("unknown block render type: " + type);
            }
        }
        blockTable.clear();
        map_name_id.clear();
        addBlock(new Air());
        gtexturemanager.init();
        for (auto & package : blockFile.GetArray()) {
            for (auto & block : package["block"].GetArray()) {
                std::string type = block["type"].GetString();
                if (type == "cube") addBlock(new Cube(block));
                else if (type == "glass") addBlock(new Glass(block));
                else if (type == "slab") addBlock(new Slab(block));
                else if (type == "stairs") addBlock(new Stairs(block));
                else if (type == "rail") addBlock(new Rail(block));
                else if (type == "grass") addBlock(new Grass(block));
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
