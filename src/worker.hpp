#ifndef worker_hpp
#define worker_hpp
#include "mesh.hpp"
namespace worker {
    void workerStart(int num);
    void workerStop();
    void submit(mineworld::MeshTask task, bool priority = false);
    std::vector<mineworld::MeshResult> takeResults(unsigned limit);
    void cancel(const glm::ivec3 & position, std::uint64_t generation);
}
#endif
