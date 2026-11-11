#pragma once

#include <cfloat>
#include <config.hpp>
#include <chunks.hpp>

using glm::dot;
using glm::floor;
using glm::normalize;

float reachdist = 5.0f;

struct Rayhit {
    bool hit;
    ivec3 pos;
    ivec3 nor;
};

struct Rayxer {
    void raycast(Rayhit &result, const vec3 &org, const vec3 &dir, Chunksys &chunksys) {
        result.hit = false;
        result.nor = ivec3{0};
        if (dot(dir, dir) <= 0.0f) return;
        vec3 ndir {normalize(dir)};
        ivec3 pos {floor(org)};
        int stepx = ndir.x > 0.0f ? 1 : (ndir.x < 0.0f ? -1 : 0);
        int stepy = ndir.y > 0.0f ? 1 : (ndir.y < 0.0f ? -1 : 0);
        int stepz = ndir.z > 0.0f ? 1 : (ndir.z < 0.0f ? -1 : 0);
        float nextx = ndir.x > 0.0f ? float(pos.x + 1) : float(pos.x);
        float nexty = ndir.y > 0.0f ? float(pos.y + 1) : float(pos.y);
        float nextz = ndir.z > 0.0f ? float(pos.z + 1) : float(pos.z);
        float tmaxx = ndir.x != 0.0f ? (nextx - org.x) / ndir.x : FLT_MAX;
        float tmaxy = ndir.y != 0.0f ? (nexty - org.y) / ndir.y : FLT_MAX;
        float tmaxz = ndir.z != 0.0f ? (nextz - org.z) / ndir.z : FLT_MAX;
        if (tmaxx < 0.0f) tmaxx = 0.0f;
        if (tmaxy < 0.0f) tmaxy = 0.0f;
        if (tmaxz < 0.0f) tmaxz = 0.0f;
        float tdeltax = ndir.x != 0.0f ? abs(1.0f / ndir.x) : FLT_MAX;
        float tdeltay = ndir.y != 0.0f ? abs(1.0f / ndir.y) : FLT_MAX;
        float tdeltaz = ndir.z != 0.0f ? abs(1.0f / ndir.z) : FLT_MAX;
        float traveled = 0.0f;
        while (traveled <= reachdist) {
            if (chunksys.getblock(pos) != 0) {
                result.hit = true;
                result.pos = pos;
                return;
            }
            if (tmaxx < tmaxy && tmaxx < tmaxz) {
                pos.x += stepx;
                traveled = tmaxx;
                tmaxx += tdeltax;
                result.nor = {-stepx, 0, 0};
            }
            else if (tmaxy < tmaxz) {
                pos.y += stepy;
                traveled = tmaxy;
                tmaxy += tdeltay;
                result.nor = {0, -stepy, 0};
            }
            else {
                pos.z += stepz;
                traveled = tmaxz;
                tmaxz += tdeltaz;
                result.nor = {0, 0, -stepz};
            }
        }
    }
};