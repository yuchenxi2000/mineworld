#include "entity.hpp"

namespace mineworld {
    
    Entity::Entity() {
        lookdirection = {1, 0, 0};
        entitypos = entity_pos_t({0, 0, 0}, {0, 0, 0});
        vc = {0, 0, 0};
        acc = {0, 0, 0};
        reachdistance = 10;
        blockhitpos = hit_none;
        entityhitpos = hit_none;
        hitentity = 0;
    }
    
    Entity::Entity(const entity_pos_t & epos) {
        lookdirection = {1, 0, 0};
        entitypos = epos;
        vc = {0, 0, 0};
        acc = {0, 0, 0};
        reachdistance = 10;
        blockhitpos = hit_none;
        entityhitpos = hit_none;
        hitentity = 0;
    }
    
    void Entity::updateLookAt() {
        blockhitpos = hit_none;
        glm::ivec3 lentityblock = {
            floorf(entitypos.offset.x),
            floorf(entitypos.offset.y),
            floorf(entitypos.offset.z)
        };
        glm::ivec3 lblock, hitblockoffset;
        hit_pos_t lhp;
        blockhitpos = hit_none;
        double d;
        double dmin = INFINITY;
        
        // x axis
        lblock = lentityblock;
        if (lookdirection.x > 0) {
            lblock.x += 1;
            while (true) {
                d = (lblock.x - entitypos.offset.x) / lookdirection.x;
                if (d > reachdistance || d < -reachdistance) {
                    break;
                }else {
                    lblock.y = floorf(entitypos.offset.y + d * lookdirection.y);
                    lblock.z = floorf(entitypos.offset.z + d * lookdirection.z);
                    int blockid = gchunk(entitypos.chunkpos + lblock);
                    Block * lb = gblockregister.getBlockbyID(blockid);
                    if (lb->hit(entitypos.offset - glm::vec3(lblock), lookdirection, lhp)) {
                        if (d < dmin) {
                            dmin = d;
                            hitblockoffset = lblock;
                            blockhitpos = lhp;
                        }
                        break;
                    }
                    lblock.x += 1;
                }
            }
        }else {
            while (true) {
                d = (lblock.x - entitypos.offset.x) / lookdirection.x;
                if (d > reachdistance || d < -reachdistance) {
                    break;
                }else {
                    lblock.y = floorf(entitypos.offset.y + d * lookdirection.y);
                    lblock.z = floorf(entitypos.offset.z + d * lookdirection.z);
                    lblock.x--;
                    int blockid = gchunk(entitypos.chunkpos + lblock);
                    Block * lb = gblockregister.getBlockbyID(blockid);
                    if (lb->hit(entitypos.offset - glm::vec3(lblock), lookdirection, lhp)) {
                        if (d < dmin) {
                            dmin = d;
                            hitblockoffset = lblock;
                            blockhitpos = lhp;
                        }
                        break;
                    }
                }
            }
        }
        
        // y axis
        lblock = lentityblock;
        if (lookdirection.y > 0) {
            lblock.y += 1;
            while (true) {
                d = (lblock.y - entitypos.offset.y) / lookdirection.y;
                if (d > reachdistance || d < -reachdistance) {
                    break;
                }else {
                    lblock.x = floorf(entitypos.offset.x + d * lookdirection.x);
                    lblock.z = floorf(entitypos.offset.z + d * lookdirection.z);
                    int blockid = gchunk(entitypos.chunkpos + lblock);
                    Block * lb = gblockregister.getBlockbyID(blockid);
                    if (lb->hit(entitypos.offset - glm::vec3(lblock), lookdirection, lhp)) {
                        if (d < dmin) {
                            dmin = d;
                            hitblockoffset = lblock;
                            blockhitpos = lhp;
                        }
                        break;
                    }
                    lblock.y += 1;
                }
            }
        }else {
            while (true) {
                d = (lblock.y - entitypos.offset.y) / lookdirection.y;
                if (d > reachdistance || d < -reachdistance) {
                    break;
                }else {
                    lblock.x = floorf(entitypos.offset.x + d * lookdirection.x);
                    lblock.z = floorf(entitypos.offset.z + d * lookdirection.z);
                    lblock.y--;
                    int blockid = gchunk(entitypos.chunkpos + lblock);
                    Block * lb = gblockregister.getBlockbyID(blockid);
                    if (lb->hit(entitypos.offset - glm::vec3(lblock), lookdirection, lhp)) {
                        if (d < dmin) {
                            dmin = d;
                            hitblockoffset = lblock;
                            blockhitpos = lhp;
                        }
                        break;
                    }
                }
            }
        }
        
        // z axis
        lblock = lentityblock;
        if (lookdirection.z > 0) {
            lblock.z += 1;
            while (true) {
                d = (lblock.z - entitypos.offset.z) / lookdirection.z;
                if (d > reachdistance || d < -reachdistance) {
                    break;
                }else {
                    lblock.y = floorf(entitypos.offset.y + d * lookdirection.y);
                    lblock.x = floorf(entitypos.offset.x + d * lookdirection.x);
                    int blockid = gchunk(entitypos.chunkpos + lblock);
                    Block * lb = gblockregister.getBlockbyID(blockid);
                    if (lb->hit(entitypos.offset - glm::vec3(lblock), lookdirection, lhp)) {
                        if (d < dmin) {
                            dmin = d;
                            hitblockoffset = lblock;
                            blockhitpos = lhp;
                        }
                        break;
                    }
                    lblock.z += 1;
                }
            }
        }else {
            while (true) {
                d = (lblock.z - entitypos.offset.z) / lookdirection.z;
                if (d > reachdistance || d < -reachdistance) {
                    break;
                }else {
                    lblock.y = floorf(entitypos.offset.y + d * lookdirection.y);
                    lblock.x = floorf(entitypos.offset.x + d * lookdirection.x);
                    lblock.z--;
                    int blockid = gchunk(entitypos.chunkpos + lblock);
                    Block * lb = gblockregister.getBlockbyID(blockid);
                    if (lb->hit(entitypos.offset - glm::vec3(lblock), lookdirection, lhp)) {
                        if (d < dmin) {
                            dmin = d;
                            hitblockoffset = lblock;
                            blockhitpos = lhp;
                        }
                        break;
                    }
                }
            }
        }
        
        hitblock = entitypos.chunkpos + hitblockoffset;
        
    }
    
    void Player::prevBlock() {
        if (holdblock > 0) {
            holdblock--;
            gboard.display(std::string(SCREEN_DISPLAY) + gblockregister.getBlockNamebyID(holdblock));
        }
    }
    
    void Player::nextBlock() {
        int maxblock = gblockregister.blockTable.size();
        if (holdblock < maxblock - 1) {
            holdblock++;
            gboard.display(std::string(SCREEN_DISPLAY) + gblockregister.getBlockNamebyID(holdblock));
        }
    }
}
