#include "entity.hpp"

namespace mineworld2 {
    Player gplayer;
    
    void Entity::nextBlock() {
        inventory.inventory[0]++;
        if (inventory.inventory[0] >= gblockregister.blockTable.size()) {
            inventory.inventory[0] = 0;
        }
        Block * block = gblockregister.blockTable[inventory.inventory[0]];
        gboard.display(std::string("MineWorld 2.1.2 holding: " + block->name));
    }
    
    void Entity::prevBlock() {
        inventory.inventory[0]--;
        if (inventory.inventory[0] < 0) {
            inventory.inventory[0] = gblockregister.blockTable.size() - 1;
        }
        Block * block = gblockregister.blockTable[inventory.inventory[0]];
        gboard.display(std::string("MineWorld 2.1.2 holding: " + block->name));
    }
}
