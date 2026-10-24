#ifndef hitbox_hpp
#define hitbox_hpp

#include <iostream>
#include <glm/glm.hpp>
#include "types.hpp"

namespace mineworld {
    struct hit_pos_t {
        int boxnum = 0;
        float distance;
        Face face;
        glm::vec3 point;
        bool operator < (const hit_pos_t & other) {
            return this->distance < other.distance;
        }
        bool operator > (const hit_pos_t & other) {
            return this->distance > other.distance;
        }
    };
    
    extern hit_pos_t hit_none;
    
    class Hitbox {
        int boxnum = 0;
        glm::vec3 vmin, vmax;
    public:
        Hitbox(glm::vec3 vmin, glm::vec3 vmax) : vmin(vmin), vmax(vmax) {}
        Hitbox(glm::vec3 vmin, glm::vec3 vmax, int num) : boxnum(num), vmin(vmin), vmax(vmax) {}
        hit_pos_t hit(const glm::vec3 & position, const glm::vec3 & direction) const;
    };
}

#endif /* hitbox_hpp */
