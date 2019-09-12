#ifndef cell_hpp
#define cell_hpp

#include <iostream>
#include <unordered_set>
#include "shader.hpp"
#include "shape.hpp"
#include "block.hpp"
#include "handler.hpp"
#include "worker.hpp"
namespace mineworld2 {
    const int CELL_X = 16, CELL_Y = 16, CELL_Z = 16, CELL_V = CELL_X * CELL_Y * CELL_Z;
    
    /*
     * an area of size (16, 16, 16)
     */
    class Cell {
    public:
        glRectBuffer glbuffer; // vertex buffer
        int blockbuffer[16 * 16 * 16];
        int nearblock[6][16][16];
        std::vector<rect> * vertexarray;
        ivec3 posoffset; // the coord of block at (0, 0, 0)
        bool GPU;
        bool rebuild;
        
        Cell() : glbuffer(), posoffset(), GPU(false), rebuild(false) {}
        Cell(const ivec3 & v) : glbuffer(), posoffset(v), GPU(false), rebuild(false) {}
        
        void setoffset(const ivec3 & v) {
            posoffset = v;
        }
        void vertexToGPU() {
            glbuffer.insert((int)vertexarray->size(), &(*vertexarray)[0]);
            GPU = true;
            rebuild = true;
        }
        void blockUpdate(const mineworld2::ivec3 & pos, int block);
        void clear() {
            glbuffer.clear();
            GPU = false;
            rebuild = false;
        }
        int operator () (int x, int y, int z) {
            return blockbuffer[x + (z << 4) + (y << 8)];
        }
        int operator () (const ivec3 & p) {
            return blockbuffer[p.x + (p.z << 4) + (p.y << 8)];
        }
        int operator () (int ipos) {
            return blockbuffer[ipos];
        }
        int upper(const ivec3 & p) {
            if (p.y < 15) return blockbuffer[p.x + (p.z << 4) + (p.y << 8) + 256];
            else return nearblock[0][p.x][p.z];
        }
        int bottom(const ivec3 & p) {
            if (p.y > 0) return blockbuffer[p.x + (p.z << 4) + (p.y << 8) - 256];
            else return nearblock[1][p.x][p.z];
        }
        int left(const ivec3 & p) {
            if (p.x > 0) return blockbuffer[p.x + (p.z << 4) + (p.y << 8) - 1];
            else return nearblock[2][p.y][p.z];
        }
        int right(const ivec3 & p) {
            if (p.x < 15) return blockbuffer[p.x + (p.z << 4) + (p.y << 8) + 1];
            else return nearblock[3][p.y][p.z];
        }
        int front(const ivec3 & p) {
            if (p.z < 15) return blockbuffer[p.x + (p.z << 4) + (p.y << 8) + 16];
            else return nearblock[4][p.x][p.y];
        }
        int back(const ivec3 & p) {
            if (p.z > 0) return blockbuffer[p.x + (p.z << 4) + (p.y << 8) - 16];
            else return nearblock[5][p.x][p.y];
        }
    };
    
    /*
     * manage chunk
     * load / remove chunk
     */
    class Chunk {
    public:
        std::unordered_map<ivec3, Cell *, ivec3_hash> chunkmap;
        std::unordered_set<ivec3, ivec3_hash> processingchunks;
        std::list<Cell *> free_list;
        static const int CELL_PER_CHUNK = 8;
        ivec3 currentchunk;
    public:
        Chunk() : currentchunk(0x7fffffff, 0x7fffffff, 0x7fffffff) {}
        void load();
        void loadCells(ivec3 & p);
        
        void updateCell(Cell * cell);
        
        // update block at pos
        void blockUpdate(const mineworld2::ivec3 & pos, int block);
        
        // query block at position (x, y, z)
        int operator () (int x, int y, int z) {
            return (*this)(ivec3(x, y, z));
        }
        int operator () (const ivec3 & pos) {
            block_loc_t blockloc = getBlockInChunk(pos);
            auto p = chunkmap.find(blockloc.chunkpos);
            if (p != chunkmap.end())
                return p->second->blockbuffer[blockloc.blockoffset.x + blockloc.blockoffset.z * CELL_X + blockloc.blockoffset.y * CELL_X * CELL_Z];
            else
                return 0;
        }
    };
    
    extern Chunk gchunk;
}

#endif /* cell_hpp */
