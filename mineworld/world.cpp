//
//  world.cpp
//  mineworld2
//
//  Created by 虞晨曦 on 2019/9/5.
//  Copyright © 2019年 虞晨曦. All rights reserved.
//

#include "world.hpp"
namespace mineworld2 {
    void WorldGenerator::init() {
        blockID.grass = gblockregister.getBlockIDbyName(std::string("grass"));
        if (blockID.grass < 0) {
            std::cerr << "[world] cannot find blockID of grass" << std::endl;
        }
    }
    void WorldGenerator::generateWorld(const ivec3 & chunkpos, Cell * cell) {
        int * blocks = cell->blockbuffer;
        memset(blocks, 0, 16 * 16 * 16 * sizeof(int));
        if (chunkpos.y == 0) {
            for (int i = 0; i < 16; ++i) {
                for (int j = 0; j < 16; ++j) {
                    blocks[i + j * 16] = blockID.grass;
                }
            }
        }
        
        memset(cell->nearblock, 0, 6 * 16 * 16 * sizeof(int));
        for (int i = 0; i < 16; ++i) {
            cell->nearblock[2][0][i] = blockID.grass;
            cell->nearblock[3][0][i] = blockID.grass;
            cell->nearblock[4][i][0] = blockID.grass;
            cell->nearblock[5][i][0] = blockID.grass;
        }
    }
    WorldGenerator gworldgenerator;
}
