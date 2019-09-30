#ifndef model_hpp
#define model_hpp

#include <iostream>
#include "shape.hpp"
#include "texture.hpp"
namespace mineworld {
    rect rectX(const glm::vec3 & start, const glm::vec3 & end, const glm::vec2 & uvstart, const glm::vec2 & uvend, const texture_loc_t & textureloc);
    rect rectY(const glm::vec3 & start, const glm::vec3 & end, const glm::vec2 & uvstart, const glm::vec2 & uvend, const texture_loc_t & textureloc);
    rect rectZ(const glm::vec3 & start, const glm::vec3 & end, const glm::vec2 & uvstart, const glm::vec2 & uvend, const texture_loc_t & textureloc);
    rect rectP(const glm::vec3 & p1, const glm::vec3 & p2, const glm::vec3 & p3, const glm::vec3 & p4, const glm::vec2 & uvstart, const glm::vec2 & uvend, const texture_loc_t & textureloc);
}

#endif /* model_hpp */
