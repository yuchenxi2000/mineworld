#ifndef block_hpp
#define block_hpp

#include <iostream>
#include <vector>
#include <map>
#include <rapidjson/document.h>
#include <rapidjson/filereadstream.h>
#include "shape.hpp"
#include "texture.hpp"
#include "cell.hpp"

namespace mineworld2 {
    class Cell;
    
    bool blockComplete(int block);
    
    /*
     * genetic block type
     */
    class Block {
    public:
        std::vector<rect> model;
        std::string name;
        bool isComplete;
        
        Block() {
            isComplete = true;
        }
        virtual void addVertex(std::vector<rect> * vertexarray, Cell * cell, ivec3 & pos) {
            for (auto & r : model) {
                vertexarray->push_back(shift(r, pos));
            }
        }
    };
    
    class Air : public Block {
    public:
        Air() {
            isComplete = false;
            name = std::string("air");
        }
        virtual void addVertex(std::vector<rect> * vertexarray, Cell * cell, ivec3 & pos) {
            
        }
    };
    
    class Cube : public Block {
    public:
        Cube(rapidjson::Value & blockinfo);
        virtual void addVertex(std::vector<rect> * vertexarray, Cell * cell, ivec3 & pos);
    };
    
    class Glass : public Cube {
    public:
        Glass(rapidjson::Value & blockinfo) : Cube(blockinfo) {
            isComplete = false;
        }
        virtual void addVertex(std::vector<rect> * vertexarray, Cell * cell, ivec3 & pos) {
            Block::addVertex(vertexarray, cell, pos);
        }
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
        int getBlockIDbyName(const std::string & name) {
            auto p = map_name_id.find(name);
            if (p == map_name_id.end()) return -1;
            return p->second;
        }
        bool blockExists(int B) {
            return B < blockTable.size() && B >= 0 && blockTable[B] != 0;
        }
        
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
