#pragma once

#include <functional>
#include <config.hpp>

struct ivec3hash {
    size_t operator()(const ivec3 &vec) const {
        size_t h1 = hash<int>()(vec.x);
        size_t h2 = hash<int>()(vec.y);
        size_t h3 = hash<int>()(vec.z);
        return h1 ^ (h2 << 1) ^ (h3 << 2);
    }
};