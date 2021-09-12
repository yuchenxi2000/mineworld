#ifndef worker_hpp
#define worker_hpp

#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <deque>
#include "util.hpp"
#include "cell.hpp"
#include "world.hpp"

/*
 * worker threads
 */
namespace worker {
    extern std::thread * th;
    extern int thread_num;
    extern std::condition_variable cv;
    extern std::mutex cv_m;
    extern std::mutex res_m;
    
    extern std::deque<void *> task_queue;
    extern std::deque<void *> result_queue;
    
    void genVertexBuffer();
    
    void workerStart(int num);
}

#endif /* worker_hpp */
