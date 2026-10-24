#ifndef world_hpp
#define world_hpp

#include <iostream>
#include "util.hpp"
#include <array>
#include "types.hpp"
namespace mineworld {
    class WorldGenerator {
        struct {
            int grass = 0;
        } blockID;
    public:
        void init();
        int blockAt(const glm::ivec3 & position) const;
        void generateWorld(const glm::ivec3 & chunkpos, std::array<int, CELL_V> & blocks) const;
    };
    extern WorldGenerator gworldgenerator;
}


#endif /* world_hpp */
