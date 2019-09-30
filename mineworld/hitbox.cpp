#include "hitbox.hpp"

namespace mineworld {
    
    hit_pos_t hit_none = {
        0, INFINITY, NONE, {0, 0, 0}
    };
    
    hit_pos_t Hitbox::hit(const glm::vec3 & position, const glm::vec3 & direction) {
        // TODO
        // -z : front; +z : back
        // +x : right; -x : left
        // +y : top; -y : bottom
        float ld;
        glm::vec3 lp; // point we look at
        hit_pos_t lhp = hit_none;
        lhp.boxnum = boxnum;
        // x direction
        if (direction.x > 0) {
            ld = (vmin.x - position.x) / direction.x;
            // caculate the coord of point
            lp = position + ld * direction;
            // if the point is in the face.
            if (lp.y >= vmin.y && lp.y <= vmax.y && lp.z >= vmin.z && lp.z <= vmax.z) {
                lhp.distance = ld;
                lhp.face = RIGHT;
                lhp.point = lp;
            }
        }else {
            ld = (vmax.x - position.x) / direction.x;
            lp = position + ld * direction;
            if (lp.y >= vmin.y && lp.y <= vmax.y && lp.z >= vmin.z && lp.z <= vmax.z) {
                lhp.distance = ld;
                lhp.face = LEFT;
                lhp.point = lp;
            }
        }
        
        // y direction
        if (direction.y > 0) {
            ld = (vmin.y - position.y) / direction.y;
            lp = position + ld * direction;
            if (lp.x >= vmin.x && lp.x <= vmax.x && lp.z >= vmin.z && lp.z <= vmax.z) {
                lhp.distance = ld;
                lhp.face = BOTTOM;
                lhp.point = lp;
            }
        }else {
            ld = (vmax.y - position.y) / direction.y;
            lp = position + ld * direction;
            if (lp.x >= vmin.x && lp.x <= vmax.x && lp.z >= vmin.z && lp.z <= vmax.z) {
                lhp.distance = ld;
                lhp.face = TOP;
                lhp.point = lp;
            }
        }
        
        // z direction
        if (direction.z > 0) {
            ld = (vmin.z - position.z) / direction.z;
            lp = position + ld * direction;
            if (lp.y >= vmin.y && lp.y <= vmax.y && lp.x >= vmin.x && lp.x <= vmax.x) {
                lhp.distance = ld;
                lhp.face = BACK;
                lhp.point = lp;
            }
        }else {
            ld = (vmax.z - position.z) / direction.z;
            lp = position + ld * direction;
            if (lp.y >= vmin.y && lp.y <= vmax.y && lp.x >= vmin.x && lp.x <= vmax.x) {
                lhp.distance = ld;
                lhp.face = FRONT;
                lhp.point = lp;
            }
        }
        
        return lhp;
    }
}
