#pragma once
#include <functional>
#include <glm/glm.hpp>

extern int screenwidth;
extern int screenheight;
extern float deltatime;

struct ivec3hash {
    std::size_t operator()(const glm::ivec3 &vec) const {
        std::size_t h1 = std::hash<int>()(vec.x);
        std::size_t h2 = std::hash<int>()(vec.y);
        std::size_t h3 = std::hash<int>()(vec.z);
        return h1 ^ (h2 << 1) ^ (h3 << 2);
    }
};