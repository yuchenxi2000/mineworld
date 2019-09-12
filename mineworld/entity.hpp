#ifndef entity_hpp
#define entity_hpp

#include <iostream>
#include <string>
#include "block.hpp"
#include "terminal.hpp"
namespace mineworld2 {
    class Inventory {
    public:
        int inventory[10];
        int chosen;
        
        Inventory() {
            memset(inventory, 0, sizeof(inventory));
            chosen = 0;
            //test
            inventory[0] = 2;
        }
        
        int getChosenBlock() {
            return inventory[chosen];
        }
    };
    class Entity {
        Inventory inventory;
    public:
        int getHoldBlock() {
            return inventory.getChosenBlock();
        }
        
        void nextBlock();
        void prevBlock();
    };
    class Player : public Entity {
        std::string playername;
    public:
        
    };
    extern Player gplayer;
}

#endif /* entity_hpp */
