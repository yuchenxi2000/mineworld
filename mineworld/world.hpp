#ifndef world_hpp
#define world_hpp

#include <iostream>
#include "util.hpp"
#include "block.hpp"
namespace mineworld2 {
    class WorldGenerator {
        struct {
            int grass;
        } blockID;
    public:
        void init();
        void generateWorld(const ivec3 & chunkpos, Cell * cell);
    };
    extern WorldGenerator gworldgenerator;
}

#endif /* world_hpp */
