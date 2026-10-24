#include "world.hpp"
#include "block.hpp"
#include <stdexcept>

namespace mineworld {
    void WorldGenerator::init() {
        blockID.grass = gblockregister.getBlockIDbyName("grass");
        if (blockID.grass < 0) throw std::runtime_error("world: cannot find grass block");
    }
    int WorldGenerator::blockAt(const glm::ivec3 & position) const {
        return position.y == 0 ? blockID.grass : 0;
    }
    void WorldGenerator::generateWorld(const glm::ivec3 & position, std::array<int, CELL_V> & blocks) const {
        blocks.fill(0);
        if (position.y == 0)
            for (int i = 0; i < CELL_X * CELL_Z; ++i) blocks[i] = blockID.grass;
    }
    WorldGenerator gworldgenerator;
}
