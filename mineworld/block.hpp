#ifndef block_hpp
#define block_hpp

#include <iostream>
#include <vector>
#include <map>
#include <rapidjson/document.h>
#include <rapidjson/filereadstream.h>
#include "shape.hpp"
#include "model.hpp"
#include "cell.hpp"
#include "terminal.hpp"
#include "hitbox.hpp"

namespace mineworld {
    class Cell;
    
    bool blockComplete(int block);
    
    /*
     * genetic block type
     */
    class Block {
    public:
        std::vector<rect> model;
        std::string name;
        int ID;
        bool isComplete;
        std::vector<Hitbox> vhitbox;
        
        Block() {
            isComplete = true;
        }
        virtual void addVertex(std::vector<rect> * vertexarray, Cell * cell, glm::ivec3 & pos) {
            for (auto & r : model) {
                vertexarray->push_back(shift(r, pos));
            }
        }
//        virtual void leftClick(const glm::ivec3 & pos, Face face);
        virtual void leftClick(const block_loc_t & pos, const hit_pos_t & hitpos, int holdblock);
        virtual void rightClick(const block_loc_t & pos, const hit_pos_t & hitpos, int holdblock);
//        virtual void rightClick(const glm::ivec3 & pos, Face face);
//        virtual void rightClick(const ivec3 & pos, const hit_pos_t & hitpos, Item * holditem);
        bool hit(const glm::vec3 & position, const glm::vec3 & direction, hit_pos_t & hitpos);
    };
    
    class Air : public Block {
    public:
        Air() {
            isComplete = false;
            name = std::string("air");
        }
        virtual void addVertex(std::vector<rect> * vertexarray, Cell * cell, glm::ivec3 & pos) {}
        virtual void leftClick(const block_loc_t & pos, const hit_pos_t & hitpos, int holdblock) {}
        virtual void rightClick(const block_loc_t & pos, const hit_pos_t & hitpos, int holdblock) {}
    };
    
    class Cube : public Block {
    public:
        Cube(rapidjson::Value & blockinfo);
        virtual void addVertex(std::vector<rect> * vertexarray, Cell * cell, glm::ivec3 & pos);
    };
    
    class Glass : public Cube {
    public:
        Glass(rapidjson::Value & blockinfo) : Cube(blockinfo) {
            isComplete = false;
        }
        virtual void addVertex(std::vector<rect> * vertexarray, Cell * cell, glm::ivec3 & pos) {
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
        std::vector<Block *> blockTable; // block ID -> generic block
        std::map<std::string, int> map_name_id; // block name -> block ID
        
        BlockRegister() {}
        ~BlockRegister() {}
        
        void loadBlock(); // load from block.json
        void addBlock(Block * block) {
            int ID = blockTable.size();
            map_name_id.insert(std::make_pair(block->name, ID));
            block->ID = ID;
            blockTable.push_back(block);
        }
        int getBlockIDbyName(const std::string & name) {
            auto p = map_name_id.find(name);
            if (p == map_name_id.end()) return -1;
            return p->second;
        }
        Block * getBlockbyID(int blockID) {
            return blockTable[blockID];
        }
        std::string & getBlockNamebyID(int blockID) {
            return blockTable[blockID]->name;
        }
        bool blockExists(int B) {
            return B < blockTable.size() && B >= 0 && blockTable[B] != 0;
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
