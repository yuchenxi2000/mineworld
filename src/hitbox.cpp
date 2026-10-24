#include "hitbox.hpp"
#include <algorithm>
#include <cmath>
#include <limits>

namespace mineworld {
    hit_pos_t hit_none = {0, std::numeric_limits<float>::infinity(), NONE, {0,0,0}};
    hit_pos_t Hitbox::hit(const glm::vec3 & origin, const glm::vec3 & direction) const {
        float entry = -std::numeric_limits<float>::infinity();
        float exit = std::numeric_limits<float>::infinity();
        Face entryFace = NONE, exitFace = NONE;
        const Face negativeFaces[] = {RIGHT, BOTTOM, BACK};
        const Face positiveFaces[] = {LEFT, TOP, FRONT};
        bool moving = false;
        for (int axis = 0; axis < 3; ++axis) {
            if (!std::isfinite(origin[axis]) || !std::isfinite(direction[axis])) return hit_none;
            if (direction[axis] == 0) {
                if (origin[axis] < vmin[axis] || origin[axis] > vmax[axis]) return hit_none;
                continue;
            }
            moving = true;
            float a = (vmin[axis] - origin[axis]) / direction[axis];
            float b = (vmax[axis] - origin[axis]) / direction[axis];
            Face nearFace = negativeFaces[axis], farFace = positiveFaces[axis];
            if (a > b) { std::swap(a,b); std::swap(nearFace,farFace); }
            if (a > entry) { entry = a; entryFace = nearFace; }
            if (b < exit) { exit = b; exitFace = farFace; }
            if (entry > exit) return hit_none;
        }
        if (!moving || exit < 0) return hit_none;
        const float distance = entry >= 0 ? entry : exit;
        return {boxnum, distance, entry >= 0 ? entryFace : exitFace, origin + distance*direction};
    }
}
