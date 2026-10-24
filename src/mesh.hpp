#ifndef mineworld_mesh_hpp
#define mineworld_mesh_hpp
#include <array>
#include <cstdint>
#include <vector>
#include <glm/glm.hpp>
#include "types.hpp"
#include "shape.hpp"

namespace mineworld {
    // Tasks own their inputs; workers never access a live Cell or chunkmap.
    struct MeshSnapshot {
        std::array<int, CELL_V> blocks{};
        int border[6][16][16]{};
        int operator()(int i) const { return blocks[i]; }
        int operator()(const glm::ivec3 & p) const { return blocks[p.x + 16*p.z + 256*p.y]; }
        int upper(const glm::ivec3 & p) const { return p.y < 15 ? blocks[p.x+16*p.z+256*p.y+256] : border[0][p.x][p.z]; }
        int bottom(const glm::ivec3 & p) const { return p.y > 0 ? blocks[p.x+16*p.z+256*p.y-256] : border[1][p.x][p.z]; }
        int left(const glm::ivec3 & p) const { return p.x < 15 ? blocks[p.x+16*p.z+256*p.y+1] : border[2][p.y][p.z]; }
        int right(const glm::ivec3 & p) const { return p.x > 0 ? blocks[p.x+16*p.z+256*p.y-1] : border[3][p.y][p.z]; }
        int front(const glm::ivec3 & p) const { return p.z < 15 ? blocks[p.x+16*p.z+256*p.y+16] : border[4][p.x][p.y]; }
        int back(const glm::ivec3 & p) const { return p.z > 0 ? blocks[p.x+16*p.z+256*p.y-16] : border[5][p.x][p.y]; }
    };
    struct MeshTask {
        glm::ivec3 position{0};
        std::uint64_t generation = 0, revision = 0;
        MeshSnapshot snapshot;
    };
    struct MeshResult {
        glm::ivec3 position{0};
        std::uint64_t generation = 0, revision = 0;
        std::vector<rect> vertices;
    };
    std::vector<rect> buildMesh(const MeshSnapshot & snapshot);
}
#endif
