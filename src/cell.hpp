#ifndef cell_hpp
#define cell_hpp
#include <memory>
#include "mesh.hpp"
#include "util.hpp"

namespace mineworld {
    class Chunk;
    class Cell {
        friend class Chunk;
        std::array<int, CELL_V> blocks{};
        std::uint64_t generation;
        std::uint64_t revision = 0;
        bool meshPending = false;
    public:
        glRectBuffer glbuffer;
        const glm::ivec3 posoffset;
        Cell(const glm::ivec3 & position, std::uint64_t generation)
            : generation(generation), posoffset(position) {}
        int operator()(const glm::ivec3 & p) const { return blocks[p.x + 16*p.z + 256*p.y]; }
        void vertexToGPU(const std::vector<rect> & vertices) {
            glbuffer.insert(static_cast<int>(vertices.size()), vertices.data());
        }
    };
    class Chunk {
        std::unordered_map<glm::ivec3, std::unique_ptr<Cell>, hash_glm_ivec3> chunkmap;
        glm::ivec3 currentchunk{0x7fffffff};
        void queueMesh(Cell & cell, bool priority);
        void updateCell(Cell & cell);
        void invalidateNeighbors(const glm::ivec3 & position);
    public:
        static const int CELL_PER_CHUNK = 8;
        ~Chunk() { clear(); }
        void load();
        void loadCells(const glm::ivec3 & position);
        MeshSnapshot snapshot(const Cell & cell) const;
        void applyResult(MeshResult result);
        void clear();
        const Cell * findCell(const glm::ivec3 & position) const {
            auto found = chunkmap.find(position);
            return found == chunkmap.end() ? nullptr : found->second.get();
        }
        bool hasPendingMeshes() const {
            for (const auto & entry : chunkmap) if (entry.second->meshPending) return true;
            return false;
        }
        void blockUpdate(const glm::ivec3 & position, int block);
        void blockUpdate(const block_loc_t & position, int block);
        int operator()(int x, int y, int z) const { return (*this)(glm::ivec3(x, y, z)); }
        int operator()(const glm::ivec3 & position) const { return (*this)(block_loc_t(position)); }
        int operator()(const block_loc_t & position) const;
    };
    extern Chunk gchunk;
}
#endif
