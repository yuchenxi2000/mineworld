#include "model.hpp"
namespace mineworld {
    void setuv(rect & r, const glm::vec2 & uvstart, const glm::vec2 & uvend, const texture_loc_t & textureloc) {
        r.v[0].uv[0] = uvstart.x; r.v[0].uv[1] = uvend.y;
        r.v[1].uv[0] = uvend.x; r.v[1].uv[1] = uvend.y;
        r.v[2].uv[0] = uvend.x; r.v[2].uv[1] = uvstart.y;
        r.v[3].uv[0] = uvstart.x; r.v[3].uv[1] = uvstart.y;
        r.v[0].uv *= config.TEXTURE_SIZE;
        r.v[1].uv *= config.TEXTURE_SIZE;
        r.v[2].uv *= config.TEXTURE_SIZE;
        r.v[3].uv *= config.TEXTURE_SIZE;
        r.v[0].uv[0] += textureloc.x; r.v[0].uv[1] += textureloc.y;
        r.v[1].uv[0] += textureloc.x; r.v[1].uv[1] += textureloc.y;
        r.v[2].uv[0] += textureloc.x; r.v[2].uv[1] += textureloc.y;
        r.v[3].uv[0] += textureloc.x; r.v[3].uv[1] += textureloc.y;
        r.v[0].uv /= gtexturemanager.MAX_TEXTURE_SIZE;
        r.v[1].uv /= gtexturemanager.MAX_TEXTURE_SIZE;
        r.v[2].uv /= gtexturemanager.MAX_TEXTURE_SIZE;
        r.v[3].uv /= gtexturemanager.MAX_TEXTURE_SIZE;
    }
    rect rectX(const glm::vec3 & start, const glm::vec3 & end, const glm::vec2 & uvstart, const glm::vec2 & uvend, const texture_loc_t & textureloc) {
        rect r;
        r.v[0].position[0] = start.x; r.v[0].position[1] = start.y; r.v[0].position[2] = start.z;
        r.v[1].position[0] = start.x; r.v[1].position[1] = start.y; r.v[1].position[2] = end.z;
        r.v[2].position[0] = start.x; r.v[2].position[1] = end.y; r.v[2].position[2] = end.z;
        r.v[3].position[0] = start.x; r.v[3].position[1] = end.y; r.v[3].position[2] = start.z;
        setuv(r, uvstart, uvend, textureloc);
        bool positive = (end.y > start.y) ^ (end.z > start.z);
        if (positive) {
            r.v[0].normal = {1, 0, 0};
            r.v[1].normal = {1, 0, 0};
            r.v[2].normal = {1, 0, 0};
            r.v[3].normal = {1, 0, 0};
        }else {
            r.v[0].normal = {-1, 0, 0};
            r.v[1].normal = {-1, 0, 0};
            r.v[2].normal = {-1, 0, 0};
            r.v[3].normal = {-1, 0, 0};
        }
        return r;
    }
    rect rectY(const glm::vec3 & start, const glm::vec3 & end, const glm::vec2 & uvstart, const glm::vec2 & uvend, const texture_loc_t & textureloc) {
        rect r;
        r.v[0].position[0] = start.x; r.v[0].position[1] = start.y; r.v[0].position[2] = start.z;
        r.v[1].position[0] = end.x; r.v[1].position[1] = start.y; r.v[1].position[2] = start.z;
        r.v[2].position[0] = end.x; r.v[2].position[1] = start.y; r.v[2].position[2] = end.z;
        r.v[3].position[0] = start.x; r.v[3].position[1] = start.y; r.v[3].position[2] = end.z;
        setuv(r, uvstart, uvend, textureloc);
        bool positive = (end.x > start.x) ^ (end.z > start.z);
        if (positive) {
            r.v[0].normal = {0, 1, 0};
            r.v[1].normal = {0, 1, 0};
            r.v[2].normal = {0, 1, 0};
            r.v[3].normal = {0, 1, 0};
        }else {
            r.v[0].normal = {0, -1, 0};
            r.v[1].normal = {0, -1, 0};
            r.v[2].normal = {0, -1, 0};
            r.v[3].normal = {0, -1, 0};
        }
        return r;
    }
    rect rectZ(const glm::vec3 & start, const glm::vec3 & end, const glm::vec2 & uvstart, const glm::vec2 & uvend, const texture_loc_t & textureloc) {
        rect r;
        r.v[0].position[0] = start.x; r.v[0].position[1] = start.y; r.v[0].position[2] = start.z;
        r.v[1].position[0] = end.x; r.v[1].position[1] = start.y; r.v[1].position[2] = start.z;
        r.v[2].position[0] = end.x; r.v[2].position[1] = end.y; r.v[2].position[2] = start.z;
        r.v[3].position[0] = start.x; r.v[3].position[1] = end.y; r.v[3].position[2] = start.z;
        setuv(r, uvstart, uvend, textureloc);
        bool positive = (end.x > start.x) ^ (end.y < start.y);
        if (positive) {
            r.v[0].normal = {0, 0, 1};
            r.v[1].normal = {0, 0, 1};
            r.v[2].normal = {0, 0, 1};
            r.v[3].normal = {0, 0, 1};
        }else {
            r.v[0].normal = {0, 0, -1};
            r.v[1].normal = {0, 0, -1};
            r.v[2].normal = {0, 0, -1};
            r.v[3].normal = {0, 0, -1};
        }
        return r;
    }
    rect rectP(const glm::vec3 & p1, const glm::vec3 & p2, const glm::vec3 & p3, const glm::vec3 & p4, const glm::vec2 & uvstart, const glm::vec2 & uvend, const texture_loc_t & textureloc) {
        rect r;
        r.v[0].position = p1;
        r.v[1].position = p2;
        r.v[2].position = p3;
        r.v[3].position = p4;
        setuv(r, uvstart, uvend, textureloc);
        glm::vec3 normal = glm::cross(p2 - p1, p3 - p2);
        
        r.v[0].normal = normal;
        r.v[1].normal = normal;
        r.v[2].normal = normal;
        r.v[3].normal = normal;
        return r;
    }
}
