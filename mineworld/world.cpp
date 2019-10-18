#include "world.hpp"
namespace mineworld {
    void WorldGenerator::init() {
        blockID.grass = gblockregister.getBlockIDbyName(std::string("grass"));
        if (blockID.grass < 0) {
            std::cerr << "[world] cannot find blockID of grass" << std::endl;
            exit(EXIT_FAILURE);
        }
    }
    void WorldGenerator::generateWorld(const glm::ivec3 & chunkpos, Cell * cell) {
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

namespace mineworld {
    unsigned int seed = 2333;
    int p[256];
    int gx[256];
    int gy[256];
    
    void noiseinit() {
        std::srand(seed);
        
        int r = rand();
        r = r & 0xff;
        for (int i = 0; i < 256; ++i) {
            p[i] = i ^ r;
            gx[i] = rand() - (RAND_MAX / 2);
            gy[i] = rand() - (RAND_MAX / 2);
        }
    }
    
    glm::vec2 rand2D(int x, int y) {
        return glm::vec2(gx[(x + p[y]) & 0xff], gy[(x + p[y]) & 0xff]);
    }
    
    glm::vec2 coffset[4] = {
        {0, 0},
        {1, 0},
        {0, 1},
        {1, 1}
    };
    
    float easefunc(float x) {
        return (3 - 2 * x) * x * x;
        //        float y = 1 + x * x * x * (x * (15 - 6 * x) - 10);
    }
    
    int clamp(int i, int min, int max) {
        int ans = i;
        if (ans < min) ans = min;
        else if (ans > max) ans = max;
        return ans;
    }
    
    float noise2D(const glm::ivec3 & c, const glm::ivec3 & p) {
        glm::vec2 g0 = glm::normalize(rand2D(c.x / 16, c.z / 16));
        glm::vec2 g1 = glm::normalize(rand2D(c.x / 16 + 1, c.z / 16));
        glm::vec2 g2 = glm::normalize(rand2D(c.x / 16, c.z / 16 + 1));
        glm::vec2 g3 = glm::normalize(rand2D(c.x / 16 + 1, c.z / 16 + 1));
        
        glm::vec2 cv = glm::vec2((float(p.x)+0.5) / 16.0, (float(p.z)+0.5) / 16.0);
        
        float s = glm::dot(g0, cv - coffset[0]);
        float t = glm::dot(g1, cv - coffset[1]);
        float u = glm::dot(g2, cv - coffset[2]);
        float v = glm::dot(g3, cv - coffset[3]);
        
        float wx = easefunc(cv.x);
        float wy = easefunc(cv.y);
        
        float a = s + wx * (t - s);
        float b = u + wx * (v - u);

        return a + wy * (b - a);
    }
    
    float noisesum(const glm::ivec3 & c, const glm::ivec3 & p) {
        glm::ivec3 cc(c);
        float res = noise2D(cc, p);
        
        glm::ivec3 ab=cc+p;
        ab *= 2;
        block_loc_t lb = ab;
        res += 1.0/3.0 * noise2D(lb.chunkpos, lb.offset);
        ab *= 2;
        lb = ab;
        res += 1.0/5.0 * noise2D(lb.chunkpos, lb.offset);
        
        return res;
    }
    
    void WorldGenerator2::init() {
        noiseinit();
        blockID.grass = gblockregister.getBlockIDbyName(std::string("grass"));
        if (blockID.grass < 0) {
            std::cerr << "[world] cannot find blockID of grass" << std::endl;
        }
        blockID.leave = gblockregister.getBlockIDbyName(std::string("leave"));
        if (blockID.leave < 0) {
            std::cerr << "[world] cannot find blockID of leave" << std::endl;
        }
        blockID.log = gblockregister.getBlockIDbyName(std::string("log"));
        if (blockID.log < 0) {
            std::cerr << "[world] cannot find blockID of log" << std::endl;
        }
    }
    void setblockincell(int * blocks, const glm::ivec3 & pos, int block) {
        if (pos.x >= 0 && pos.x < 16 && pos.y >= 0 && pos.y < 16 && pos.z >= 0 && pos.z < 16)
            blocks[pos.x + pos.z * 16 + pos.y * 16 * 16] = block;
    }
    glm::ivec3 trunk[] = {
        glm::ivec3(0, 1, 0), glm::ivec3(0, 2, 0), glm::ivec3(0, 3, 0), glm::ivec3(0, 4, 0),
    };
    glm::ivec3 leaves[] = {
        glm::ivec3(-1, 4, -1),glm::ivec3(-1, 4, 0),glm::ivec3(-1, 4, 1),
        glm::ivec3(0, 4, -1),glm::ivec3(0, 4, 1),
        glm::ivec3(1, 4, -1),glm::ivec3(1, 4, 0),glm::ivec3(1, 4, 1),
        
        glm::ivec3(-1, 5, 0),glm::ivec3(0, 5, -1),
        glm::ivec3(0, 5, 0),glm::ivec3(0, 5, 1),glm::ivec3(1, 5, 0),
    };
    void WorldGenerator2::generateTree(int * blocks, const glm::ivec3 & pos) {
        glm::ivec3 p(pos);
        for (int i = 0; i < sizeof(trunk) / sizeof(glm::ivec3); ++i) {
            setblockincell(blocks, p + trunk[i], blockID.log);
        }
        for (int i = 0; i < sizeof(leaves) / sizeof(glm::ivec3); ++i) {
            setblockincell(blocks, p + leaves[i], blockID.leave);
        }
    }
    void WorldGenerator2::generateWorld(const glm::ivec3 & chunkpos, Cell * cell) {
        int * blocks = cell->blockbuffer;
        memset(blocks, 0, 16 * 16 * 16 * sizeof(int));
        if (chunkpos.y == 0) {
            glm::ivec3 p;
            int hb[16 * 16];
            for (p.x = 0; p.x < 16; ++p.x) {
                for (p.z = 0; p.z < 16; ++p.z) {
                    float n = noisesum(chunkpos, p);
                    int r = clamp((n + 1) * 6, 0, 15);
                    for (p.y = r; p.y >= 0; --p.y) {
                        blocks[p.x + p.z * 16 + p.y * 16 * 16] = blockID.grass;
                    }
                    hb[p.x + p.z * 16] = r;
                }
            }
            int tree = rand();
            if (tree % 3 == 0) {
                glm::ivec3 tp;
                glm::vec2 rv = rand2D(chunkpos.x, chunkpos.z);
                tp.x = 2 + (int)rv.x % 12;
                tp.z = 2 + (int)rv.y % 12;
                tp.y = hb[tp.x + tp.z * 16];
                generateTree(blocks, tp);
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
    WorldGenerator2 gworldgenerator2;
}
