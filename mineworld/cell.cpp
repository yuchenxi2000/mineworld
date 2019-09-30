#include "cell.hpp"

namespace mineworld {
    Chunk gchunk;
    
    void Cell::blockUpdate(const glm::ivec3 & pos, int block) {
        if (GPU) {
            int ipos = pos.x + pos.z * CELL_X + pos.y * CELL_X * CELL_Z;
            blockbuffer[ipos] = block;
            gchunk.updateCell(this);
            
            if (pos.y == 0) {
                auto p = gchunk.chunkmap.find(posoffset - glm::ivec3(0, 16, 0));
                if (p != gchunk.chunkmap.end()) {
                    p->second->nearblock[0][pos.x][pos.z] = block;
                    gchunk.updateCell(p->second);
                }
            }else if (pos.y == 15) {
                auto p = gchunk.chunkmap.find(posoffset + glm::ivec3(0, 16, 0));
                if (p != gchunk.chunkmap.end()) {
                    p->second->nearblock[1][pos.x][pos.z] = block;
                    gchunk.updateCell(p->second);
                }
            }
            
            if (pos.x == 0) {
                auto p = gchunk.chunkmap.find(posoffset - glm::ivec3(16, 0, 0));
                if (p != gchunk.chunkmap.end()) {
                    p->second->nearblock[2][pos.y][pos.z] = block;
                    gchunk.updateCell(p->second);
                }
            }else if (pos.x == 15) {
                auto p = gchunk.chunkmap.find(posoffset + glm::ivec3(16, 0, 0));
                if (p != gchunk.chunkmap.end()) {
                    p->second->nearblock[3][pos.y][pos.z] = block;
                    gchunk.updateCell(p->second);
                }
            }
            
            if (pos.z == 0) {
                auto p = gchunk.chunkmap.find(posoffset - glm::ivec3(0, 0, 16));
                if (p != gchunk.chunkmap.end()) {
                    p->second->nearblock[4][pos.x][pos.y] = block;
                    gchunk.updateCell(p->second);
                }
            }else if (pos.z == 15) {
                auto p = gchunk.chunkmap.find(posoffset + glm::ivec3(0, 0, 16));
                if (p != gchunk.chunkmap.end()) {
                    p->second->nearblock[5][pos.x][pos.y] = block;
                    gchunk.updateCell(p->second);
                }
            }
            
            printf("[blockupdate] %d %d %d B %d\n", pos.x, pos.y, pos.z, block);

        }
    }
    
    void Chunk::updateCell(Cell * cell) {
        cell->GPU = false;
        {
            std::lock_guard<std::mutex> lkg(worker::cv_m);
            worker::task_queue.push_front(cell);
        }
        worker::cv.notify_one();
    }

    void Chunk::loadCells(glm::ivec3 & v) {
        for (v.y = 0; v.y < CELL_PER_CHUNK * CELL_Y; v.y += CELL_Y) {
            auto p = chunkmap.find(v);
            if (p == chunkmap.end()) {
                if (processingchunks.find(v) == processingchunks.end()) {
                    if (!free_list.empty()) {
                        Cell * cell = free_list.front();
                        cell->posoffset = v;
                        {
                            std::lock_guard<std::mutex> lkg(worker::cv_m);
                            worker::task_queue.push_back(cell);
                        }
                        worker::cv.notify_one();
                        
                        processingchunks.insert(v);
                        free_list.pop_front();
                    }else {
                        Cell * cell = new Cell(v);
                        {
                            std::lock_guard<std::mutex> lkg(worker::cv_m);
                            worker::task_queue.push_back(cell);
                        }
                        worker::cv.notify_one();
                        
                        processingchunks.insert(v);
                    }
                }
            }
        }
    }
    
    void Chunk::load() {
        const glm::ivec3 & coffset = handler.player->entitypos.chunkpos;
        for (auto p = chunkmap.begin(); p != chunkmap.end(); ++p) {
            gblockshader.drawCell(p->second);
        }
        
        if (coffset != currentchunk) {
            currentchunk = coffset;
            for (auto p = chunkmap.begin(); p != chunkmap.end();) {
                int x = int(p->second->posoffset.x);
                int z = int(p->second->posoffset.z);
                if (abs(x - coffset.x) > config.CHUNK_CACHE_DISTANCE * CELL_X || abs(z - coffset.z) > config.CHUNK_CACHE_DISTANCE * CELL_Z) {
                    if (!p->second->GPU) {
                        ++p;
                        continue;
                    }
                    p->second->clear();
                    free_list.push_back(p->second);
                    chunkmap.erase(p++);
                }else {
                    ++p;
                }
            }
            glm::ivec3 v;
            loadCells(currentchunk);
            for (int d = 1; d <= config.VISIBLE_DISTANCE; ++d) {
                v.x = currentchunk.x + d * CELL_X;
                for (v.z = currentchunk.z - d * CELL_Z; v.z < currentchunk.z + d * CELL_Z; v.z += CELL_Z) {
                    loadCells(v);
                }
                v.z = currentchunk.z + d * CELL_Z;
                for (v.x = currentchunk.x + d * CELL_X; v.x > currentchunk.x - d * CELL_X; v.x -= CELL_X) {
                    loadCells(v);
                }
                v.x = currentchunk.x - d * CELL_X;
                for (v.z = currentchunk.z + d * CELL_Z; v.z > currentchunk.z - d * CELL_Z; v.z -= CELL_Z) {
                    loadCells(v);
                }
                v.z = currentchunk.z - d * CELL_Z;
                for (v.x = currentchunk.x - d * CELL_X; v.x < currentchunk.x + d * CELL_X; v.x += CELL_X) {
                    loadCells(v);
                }
            }
        }
        
        const int MAXCHUNKUPDATE = 10;
        std::deque<void *> res;
        {
            std::lock_guard<std::mutex> lkg(worker::res_m);
            for (int cnt = 0; !worker::result_queue.empty() && cnt < MAXCHUNKUPDATE; ++cnt) {
                res.push_back(worker::result_queue.front());
                worker::result_queue.pop_front();
            }
        }
        
        for (auto & ret : res) {
            Cell * cell = (Cell *)ret;
            cell->vertexToGPU();
            delete cell->vertexarray;
            glm::ivec3 v = cell->posoffset;
            chunkmap.insert(std::make_pair(v, cell));
            auto p = processingchunks.find(v);
            if (p != processingchunks.end()) processingchunks.erase(p);
        }
        
    }
    
    void Chunk::blockUpdate(const glm::ivec3 & pos, int block) {
        block_loc_t blockloc = pos;
        auto p = chunkmap.find(blockloc.chunkpos);
        if (p != chunkmap.end()) {
            p->second->blockUpdate(blockloc.offset, block);
        }else {
            gterminal.println(std::string("error: unload chunk"));
        }
    }
    
    void Chunk::blockUpdate(const block_loc_t & blockpos, int block) {
        auto p = chunkmap.find(blockpos.chunkpos);
        if (p != chunkmap.end()) {
            p->second->blockUpdate(blockpos.offset, block);
        }else {
            gterminal.println(std::string("error: unload chunk"));
        }
    }
    
    int Chunk::operator () (const glm::ivec3 & pos) {
        block_loc_t blockloc = pos;
        auto p = chunkmap.find(blockloc.chunkpos);
        if (p != chunkmap.end())
            return p->second->blockbuffer[blockloc.offset.x + blockloc.offset.z * CELL_X + blockloc.offset.y * CELL_X * CELL_Z];
        else
            return 0;
    }
    
    int Chunk::operator () (const block_loc_t & blockloc) {
        auto p = chunkmap.find(blockloc.chunkpos);
        if (p != chunkmap.end())
            return p->second->blockbuffer[blockloc.offset.x + blockloc.offset.z * CELL_X + blockloc.offset.y * CELL_X * CELL_Z];
        else
            return 0;
    }

}
