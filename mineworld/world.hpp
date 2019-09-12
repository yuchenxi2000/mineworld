//
//  world.hpp
//  mineworld2
//
//  Created by 虞晨曦 on 2019/9/5.
//  Copyright © 2019年 虞晨曦. All rights reserved.
//

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
