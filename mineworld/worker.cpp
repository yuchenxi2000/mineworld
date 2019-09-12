#include "worker.hpp"
using namespace mineworld2;
namespace worker {
    std::thread * th;
    int thread_num;
    std::condition_variable cv;
    std::mutex cv_m;
    std::mutex res_m;
    
    std::deque<void *> task_queue;
    std::deque<void *> result_queue;
    
    void workerStart(int num) {
        thread_num = num;
        th = new std::thread[num];
        for (int i = 0; i < num; ++i) {
            th[i] = std::thread(genVertexBuffer);
            th[i].detach();
        }
    }
    
    void genVertexBuffer() {
        while (true) {
            std::unique_lock<std::mutex> lk(cv_m);
            cv.wait(lk, [] {return !task_queue.empty();});
            Cell * cell = (Cell *)task_queue.front();
            task_queue.pop_front();
            lk.unlock();
            
            std::vector<rect> * vertexarray = new std::vector<rect>;
            vertexarray->reserve(CELL_X * CELL_Z * CELL_Y / 2);
            
            if (!cell->rebuild)
                gworldgenerator.generateWorld(cell->posoffset, cell);
            
            int cnt = 0;
            ivec3 pos;
            int & x = pos.x;
            int & y = pos.y;
            int & z = pos.z;
            for (y = 0; y < CELL_Y; ++y) {
                for (z = 0; z < CELL_Z; ++z) {
                    for (x = 0; x < CELL_X; ++x) {
                        gblockregister.blockTable[(*cell)(cnt)]->addVertex(vertexarray, cell, pos);
                        ++cnt;
                    }
                }
            }
            
            cell->vertexarray = vertexarray;
            
            {
                std::lock_guard<std::mutex> lkg(res_m);
                result_queue.push_back(cell);
            }
            
        }
    } /* genVertexBuffer */
    
}

