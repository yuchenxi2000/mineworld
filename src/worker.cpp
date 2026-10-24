#include "worker.hpp"
#include "block.hpp"
#include <algorithm>
#include <condition_variable>
#include <deque>
#include <mutex>
#include <thread>

namespace mineworld {
    std::vector<rect> buildMesh(const MeshSnapshot & snapshot) {
        std::vector<rect> vertices;
        vertices.reserve(CELL_V / 2);
        glm::ivec3 p;
        for (p.y = 0; p.y < CELL_Y; ++p.y)
            for (p.z = 0; p.z < CELL_Z; ++p.z)
                for (p.x = 0; p.x < CELL_X; ++p.x)
                    gblockregister.getBlockbyID(snapshot(p))->addVertex(&vertices, &snapshot, p);
        return vertices;
    }
}
namespace worker {
    namespace {
        std::vector<std::thread> threads;
        std::mutex mutex;
        std::condition_variable cv;
        std::deque<mineworld::MeshTask> tasks;
        std::deque<mineworld::MeshResult> results;
        bool stopping = true;
        void run() {
            for (;;) {
                mineworld::MeshTask task;
                {
                    std::unique_lock<std::mutex> lock(mutex);
                    cv.wait(lock, [] { return stopping || !tasks.empty(); });
                    if (stopping) return;
                    task = std::move(tasks.front());
                    tasks.pop_front();
                }
                mineworld::MeshResult result;
                result.position = task.position;
                result.generation = task.generation;
                result.revision = task.revision;
                result.vertices = mineworld::buildMesh(task.snapshot);
                {
                    std::lock_guard<std::mutex> lock(mutex);
                    if (!stopping) results.push_back(std::move(result));
                }
            }
        }
    }
    void workerStart(int num) {
        workerStop();
        {
            std::lock_guard<std::mutex> lock(mutex);
            stopping = false;
        }
        try {
            for (int i = 0; i < std::max(1, num); ++i) threads.emplace_back(run);
        } catch (...) {
            workerStop();
            throw;
        }
    }
    void workerStop() {
        {
            std::lock_guard<std::mutex> lock(mutex);
            stopping = true;
            tasks.clear();
        }
        cv.notify_all();
        for (auto & thread : threads) thread.join();
        threads.clear();
        std::lock_guard<std::mutex> lock(mutex);
        results.clear();
    }
    void submit(mineworld::MeshTask task, bool priority) {
        {
            std::lock_guard<std::mutex> lock(mutex);
            if (stopping) return;
            if (priority) tasks.push_front(std::move(task));
            else tasks.push_back(std::move(task));
        }
        cv.notify_one();
    }
    std::vector<mineworld::MeshResult> takeResults(unsigned limit) {
        std::vector<mineworld::MeshResult> out;
        std::lock_guard<std::mutex> lock(mutex);
        while (!results.empty() && out.size() < limit) {
            out.push_back(std::move(results.front()));
            results.pop_front();
        }
        return out;
    }
    void cancel(const glm::ivec3 & position, std::uint64_t generation) {
        std::lock_guard<std::mutex> lock(mutex);
        auto matches = [&](const auto & item) { return item.position == position && item.generation == generation; };
        tasks.erase(std::remove_if(tasks.begin(), tasks.end(), matches), tasks.end());
        results.erase(std::remove_if(results.begin(), results.end(), matches), results.end());
        // Running tasks own their data; late results are rejected by generation.
    }
}
