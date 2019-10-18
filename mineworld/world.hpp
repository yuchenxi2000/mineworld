#ifndef world_hpp
#define world_hpp

#include <iostream>
#include "util.hpp"
#include "block.hpp"
namespace mineworld {
    class WorldGenerator {
        struct {
            int grass;
        } blockID;
    public:
        void init();
        void generateWorld(const glm::ivec3 & chunkpos, Cell * cell);
    };
    extern WorldGenerator gworldgenerator;
}

namespace mineworld {
    class WorldGenerator2 {
        struct {
            int grass, log, leave;
        } blockID;
    public:
        void init();
        void generateTree(int * blocks, const glm::ivec3 & pos);
        void generateWorld(const glm::ivec3 & chunkpos, Cell * cell);
    };
    extern WorldGenerator2 gworldgenerator2;
}

#endif /* world_hpp */
