#include "entity.hpp"
#include "cell.hpp"
#include "block.hpp"
#include "terminal.hpp"
#include <algorithm>
#include <cmath>
#include <limits>

namespace mineworld {
    Entity::Entity() : Entity(entity_pos_t({0,0,0}, {0,0,0})) {}
    Entity::Entity(const entity_pos_t & position)
        : lookdirection(1,0,0), entitypos(position), vc(0), acc(0), reachdistance(10),
          blockhitpos(hit_none), hitblock(static_cast<block_loc_t>(position)),
          entityhitpos(hit_none), hitentity(nullptr) {}

    void Entity::updateLookAt() {
        blockhitpos = hit_none;
        hitblock = static_cast<block_loc_t>(entitypos);
        const float length = glm::length(lookdirection);
        if (!std::isfinite(length) || length == 0 || !std::isfinite(reachdistance) || reachdistance < 0) return;
        const glm::vec3 direction = lookdirection / length;
        const glm::vec3 origin = entitypos.offset;
        glm::ivec3 voxel(glm::floor(origin)), step(0);
        glm::vec3 next(std::numeric_limits<float>::infinity());
        glm::vec3 delta(std::numeric_limits<float>::infinity());
        for (int axis = 0; axis < 3; ++axis) {
            if (direction[axis] == 0) continue;
            step[axis] = direction[axis] > 0 ? 1 : -1;
            const float boundary = static_cast<float>(voxel[axis] + (step[axis] > 0 ? 1 : 0));
            next[axis] = (boundary - origin[axis]) / direction[axis];
            delta[axis] = std::abs(1.0f / direction[axis]);
        }
        float travelled = 0;
        while (travelled <= reachdistance) {
            const float crossing = std::min({next.x,next.y,next.z});
            const int id = gchunk(entitypos.chunkpos + voxel);
            hit_pos_t hit;
            if (gblockregister.getBlockbyID(id)->hit(origin-glm::vec3(voxel), direction, hit) &&
                hit.distance >= travelled - 0.00001f && hit.distance <= crossing + 0.00001f &&
                hit.distance <= reachdistance) {
                blockhitpos = hit;
                hitblock = block_loc_t(entitypos.chunkpos + voxel);
                return;
            }
            if (!std::isfinite(crossing)) return;
            for (int axis = 0; axis < 3; ++axis)
                if (next[axis] == crossing) { voxel[axis] += step[axis]; next[axis] += delta[axis]; }
            travelled = crossing;
        }
    }
    void Player::prevBlock() {
        if (holdblock > 0) setHoldBlock(holdblock-1);
    }
    void Player::nextBlock() {
        if (static_cast<size_t>(holdblock+1) < gblockregister.blockTable.size()) setHoldBlock(holdblock+1);
    }
    void Player::setHoldBlock(int block) {
        if (!gblockregister.blockExists(block)) return;
        holdblock = block;
        gboard.display(std::string(SCREEN_DISPLAY) + gblockregister.getBlockNamebyID(holdblock));
    }
}
