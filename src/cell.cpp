#include "cell.hpp"
#include "block.hpp"
#include "handler.hpp"
#include "shader.hpp"
#include "terminal.hpp"
#include "worker.hpp"
#include "world.hpp"

namespace mineworld {
    Chunk gchunk;
    namespace {
        std::uint64_t nextGeneration = 1;
        const glm::ivec3 directions[] = {{0,16,0}, {0,-16,0}, {16,0,0}, {-16,0,0}, {0,0,16}, {0,0,-16}};
    }
    MeshSnapshot Chunk::snapshot(const Cell & cell) const {
        MeshSnapshot out;
        out.blocks = cell.blocks;
        const Cell * neighbors[6]{};
        for (int face = 0; face < 6; ++face) neighbors[face] = findCell(cell.posoffset + directions[face]);
        for (int a = 0; a < 16; ++a) {
            for (int b = 0; b < 16; ++b) {
                const glm::ivec3 local[] = {{a,0,b},{a,15,b},{0,a,b},{15,a,b},{a,b,0},{a,b,15}};
                for (int face = 0; face < 6; ++face)
                    out.border[face][a][b] = neighbors[face] ? (*neighbors[face])(local[face]) :
                        gworldgenerator.blockAt(cell.posoffset + directions[face] + local[face]);
            }
        }
        return out;
    }
    void Chunk::queueMesh(Cell & cell, bool priority) {
        if (cell.meshPending) return;
        MeshTask task;
        task.position = cell.posoffset;
        task.generation = cell.generation;
        task.revision = cell.revision;
        task.snapshot = snapshot(cell);
        worker::submit(std::move(task), priority);
        cell.meshPending = true;
    }
    void Chunk::updateCell(Cell & cell) {
        ++cell.revision;
        queueMesh(cell, true);
    }
    void Chunk::invalidateNeighbors(const glm::ivec3 & position) {
        for (const auto & direction : directions) {
            auto found = chunkmap.find(position + direction);
            if (found != chunkmap.end()) updateCell(*found->second);
        }
    }
    void Chunk::loadCells(const glm::ivec3 & position) {
        for (int y = 0; y < CELL_PER_CHUNK * CELL_Y; y += CELL_Y) {
            glm::ivec3 p(position.x, y, position.z);
            if (chunkmap.find(p) != chunkmap.end()) continue;
            auto cell = std::make_unique<Cell>(p, nextGeneration++);
            gworldgenerator.generateWorld(p, cell->blocks);
            Cell & added = *cell;
            chunkmap.emplace(p, std::move(cell));
            queueMesh(added, false);
            // New cells match the procedural fallback used by existing snapshots.
        }
    }
    void Chunk::applyResult(MeshResult result) {
        auto found = chunkmap.find(result.position);
        if (found == chunkmap.end() || found->second->generation != result.generation) return;
        Cell & cell = *found->second;
        cell.meshPending = false;
        if (result.revision != cell.revision) {
            queueMesh(cell, true);
            return;
        }
        cell.vertexToGPU(result.vertices);
    }
    void Chunk::load() {
        const glm::ivec3 & position = handler.player->entitypos.chunkpos;
        if (position != currentchunk) {
            currentchunk = position;
            std::vector<glm::ivec3> removed;
            for (auto it = chunkmap.begin(); it != chunkmap.end();) {
                const auto & p = it->first;
                if (std::abs(static_cast<long long>(p.x) - position.x) > config.CHUNK_CACHE_DISTANCE * 16LL ||
                    std::abs(static_cast<long long>(p.z) - position.z) > config.CHUNK_CACHE_DISTANCE * 16LL) {
                    worker::cancel(p, it->second->generation);
                    removed.push_back(p);
                    it = chunkmap.erase(it);
                } else ++it;
            }
            for (const auto & p : removed) invalidateNeighbors(p);
            loadCells(currentchunk);
            for (int distance = 1; distance <= config.VISIBLE_DISTANCE; ++distance) {
                for (int x = -distance; x <= distance; ++x)
                    for (int z = -distance; z <= distance; ++z)
                        if (std::abs(x) == distance || std::abs(z) == distance)
                            loadCells(currentchunk + glm::ivec3(x*16, 0, z*16));
            }
        }
        for (auto & result : worker::takeResults(10)) applyResult(std::move(result));
        for (const auto & entry : chunkmap) gblockshader.drawCell(entry.second.get());
    }
    void Chunk::clear() {
        for (const auto & entry : chunkmap) worker::cancel(entry.first, entry.second->generation);
        chunkmap.clear();
        currentchunk = glm::ivec3(0x7fffffff);
    }
    void Chunk::blockUpdate(const glm::ivec3 & position, int block) { blockUpdate(block_loc_t(position), block); }
    void Chunk::blockUpdate(const block_loc_t & position, int block) {
        auto found = chunkmap.find(position.chunkpos);
        if (found == chunkmap.end()) { gterminal.println("error: unload chunk"); return; }
        if (!gblockregister.blockExists(block)) { gterminal.println("error: block not found"); return; }
        const auto & p = position.offset;
        if (p.x < 0 || p.x >= 16 || p.y < 0 || p.y >= 16 || p.z < 0 || p.z >= 16) return;
        Cell & cell = *found->second;
        int & current = cell.blocks[p.x + 16*p.z + 256*p.y];
        if (current == block) return;
        current = block;
        updateCell(cell);
        for (int axis = 0; axis < 3; ++axis) {
            if (p[axis] != 0 && p[axis] != 15) continue;
            glm::ivec3 neighbor(position.chunkpos);
            neighbor[axis] += p[axis] == 0 ? -16 : 16;
            auto adjacent = chunkmap.find(neighbor);
            if (adjacent != chunkmap.end()) updateCell(*adjacent->second);
        }
    }
    int Chunk::operator()(const block_loc_t & position) const {
        auto found = chunkmap.find(position.chunkpos);
        return found != chunkmap.end() ? (*found->second)(position.offset) : 0;
    }
}
