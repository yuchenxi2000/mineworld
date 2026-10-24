#ifndef entity_hpp
#define entity_hpp

#include <iostream>
#include <string>
#include "util.hpp"
#include "types.hpp"
#include "hitbox.hpp"

namespace mineworld {
    
    class Entity {
    public:
        
        glm::vec3 lookdirection;
        entity_pos_t entitypos;
        
        glm::vec3 vc; // velocity
        glm::vec3 acc; // acceleration
        
        double reachdistance;
        
        hit_pos_t blockhitpos;
        block_loc_t hitblock;
        
        hit_pos_t entityhitpos;
        Entity * hitentity;
    public:
        Entity();
        Entity(const entity_pos_t & epos);
        virtual ~Entity() = default;
        
        void updateLookAt();
    };
    
    class Player : public Entity {
        std::string playername;
        // test
        int holdblock;
    public:
        Player() {
            holdblock = 0;
        }
        Player(const std::string & name, const entity_pos_t & epos) : Entity(epos) {
            holdblock = 0;
            playername = name;
        }
        // test
        void prevBlock();
        void nextBlock();
        int getHoldBlock() {
            return holdblock;
        }
        void setHoldBlock(int block);
    };
    
}

#endif /* entity_hpp */
