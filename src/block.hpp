#ifndef block_hpp
#define block_hpp

#include <iostream>
#include <vector>
#include <map>
#include <memory>
#include <stdexcept>
#include <rapidjson/document.h>
#include "shape.hpp"
#include "model.hpp"
#include "mesh.hpp"
#include "hitbox.hpp"

namespace mineworld {
    
    bool blockComplete(int block);
    
    /*
     * generic block type
     */
    class Block {
    public:
        std::vector<rect> model;
        std::string name;
        int ID = 0;
        bool isComplete;
        std::vector<Hitbox> vhitbox;
//        std::vector<CollisionBox> vcollision;
        
        Block() {
            isComplete = true;
        }
        virtual ~Block() = default;
        virtual void addVertex(std::vector<rect> * vertexarray, const MeshSnapshot *, const glm::ivec3 & pos) const {
            for (auto & r : model) {
                vertexarray->push_back(shift(r, pos));
            }
        }
        virtual void leftClick(const block_loc_t & pos, const hit_pos_t & hitpos, int holdblock);
        virtual void rightClick(const block_loc_t & pos, const hit_pos_t & hitpos, int holdblock);
        bool hit(const glm::vec3 & position, const glm::vec3 & direction, hit_pos_t & hitpos) const;
    };
    
    class Air : public Block {
    public:
        Air() {
            isComplete = false;
            name = std::string("air");
        }
        void addVertex(std::vector<rect> *, const MeshSnapshot *, const glm::ivec3 &) const override {}
        void leftClick(const block_loc_t &, const hit_pos_t &, int) override {}
        void rightClick(const block_loc_t &, const hit_pos_t &, int) override {}
    };
    
    class Cube : public Block {
    public:
        Cube(rapidjson::Value & blockinfo);
        void addVertex(std::vector<rect> * vertexarray, const MeshSnapshot * cell, const glm::ivec3 & pos) const override;
    };
    
    class Glass : public Cube {
    public:
        Glass(rapidjson::Value & blockinfo) : Cube(blockinfo) {
            isComplete = false;
        }
        void addVertex(std::vector<rect> * vertexarray, const MeshSnapshot * cell, const glm::ivec3 & pos) const override {
            Block::addVertex(vertexarray, cell, pos);
        }
    };
    
    class Slab : public Block {
    public:
        Slab(rapidjson::Value & blockinfo);
    };
    
    class Stairs : public Block {
    public:
        Stairs(rapidjson::Value & blockinfo);
    };
    
    class Rail : public Block {
    public:
        Rail(rapidjson::Value & blockinfo);
    };
    
    class Grass : public Block {
    public:
        Grass(rapidjson::Value & blockinfo);
    };
    
    /*
     * register block from block file
     */
    class BlockRegister {
    public:
        std::vector<std::unique_ptr<Block>> blockTable; // block ID -> owned block definition
        std::map<std::string, int> map_name_id; // block name -> block ID
        
        BlockRegister() {}
        
        void loadBlock(); // load from block.json
        void addBlock(Block * block) {
            std::unique_ptr<Block> owned(block);
            if (!block || map_name_id.count(block->name)) throw std::runtime_error("duplicate or null block");
            int ID = static_cast<int>(blockTable.size());
            map_name_id.insert(std::make_pair(block->name, ID));
            block->ID = ID;
            blockTable.push_back(std::move(owned));
        }
        int getBlockIDbyName(const std::string & name) {
            auto p = map_name_id.find(name);
            if (p == map_name_id.end()) return -1;
            return p->second;
        }
        Block * getBlockbyID(int blockID) {
            if (!blockExists(blockID)) throw std::out_of_range("invalid block ID");
            return blockTable[blockID].get();
        }
        std::string & getBlockNamebyID(int blockID) {
            return getBlockbyID(blockID)->name;
        }
        bool blockExists(int B) {
            return B >= 0 && static_cast<size_t>(B) < blockTable.size() && blockTable[B] != nullptr;
        }
        void listBlock();
        
        // debug
        void debugPrint() {
            int len = (int)blockTable.size();
            for (int i = 0; i < len; ++i) {
                std::cout << "block id " << i << std::endl;
                std::vector<rect> & model = blockTable[i]->model;
                for (auto & r : model) {
                    r.print();
                }
            }
        }
        
    };
    
    extern BlockRegister gblockregister;
    
}

#endif /* block_hpp */
