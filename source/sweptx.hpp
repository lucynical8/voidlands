#pragma once

#include <limits>
#include <config.hpp>
#include <chunks.hpp>

using glm::ceil;
using glm::floor;
using glm::length;

struct Colres {
    bool floor = false;
    bool ceil = false;
    bool x = false;
    bool z = false;
};

struct Sweptx {
    bool aabbinters(const vec3 &amin, const vec3 &amax, const vec3 &bmin, const vec3 &bmax, const vec3 &vel, vec3 &nor, float &coltime) {
        coltime = 1.0f;
        nor = vec3{0.0f};
        if (vel == vec3{0.0f}) {
            bool overlap = (amin.x < bmax.x && amax.x > bmin.x) &&
						   (amin.y < bmax.y && amax.y > bmin.y) &&
						   (amin.z < bmax.z && amax.z > bmin.z);
            if (!overlap) return false;
            vec3 pen {min(amax.x - bmin.x, bmax.x - amin.x),
                      min(amax.y - bmin.y, bmax.y - amin.y),
                      min(amax.z - bmin.z, bmax.z - amin.z)};
            if (pen.x < pen.y && pen.x < pen.z) {
                nor.x = (amax.x - bmin.x) < (bmax.x - amin.x) ? -1.0f : 1.0f;
            } else if (pen.y < pen.z) {
                nor.y = (amax.y - bmin.y) < (bmax.y - amin.y) ? -1.0f : 1.0f;
            } else {
                nor.z = (amax.z - bmin.z) < (bmax.z - amin.z) ? -1.0f : 1.0f;
            }
            coltime = 0.0f;
            return true;
        }
        vec3 tentry;
        vec3 texit;
        for (int i = 0; i < 3; i++) {
            if (vel[i] == 0.0f) {
                if (amax[i] <= bmin[i] || amin[i] >= bmax[i]) return false;
                tentry[i] = -numeric_limits<float>::infinity();
                texit[i] = numeric_limits<float>::infinity();
            } else if (vel[i] > 0.0f) {
                tentry[i] = (bmin[i] - amax[i]) / vel[i];
                texit[i]  = (bmax[i] - amin[i]) / vel[i];
            } else {
                tentry[i] = (bmax[i] - amin[i]) / vel[i];
                texit[i]  = (bmin[i] - amax[i]) / vel[i];
            }
        }
        float entrytime = max({tentry.x, tentry.y, tentry.z});
        float exittime  = min({texit.x, texit.y, texit.z});
        if (entrytime > exittime || entrytime < 0.0f || entrytime > 1.0f) return false;
        if (entrytime == tentry.x) {
            nor.x = vel.x >= 0.0f ? -1.0f : 1.0f;
        } else if (entrytime == tentry.y) {
            nor.y = vel.y >= 0.0f ? -1.0f : 1.0f;
        } else if (entrytime == tentry.z) {
            nor.z = vel.z >= 0.0f ? -1.0f : 1.0f;
        }
        coltime = entrytime;
        return true;
    }

    Colres resolblocks(vec3 &pos, const vec3 &vel, const vec3 &hitboxmin, const vec3 &hitboxmax, Chunksys &chunksys) {
        Colres colres;
        if (vel == vec3{0.0f}) return colres;
        vec3 rvel {vel};
        vec3 startpos {pos};
        vec3 objminstart {startpos + hitboxmin};
        vec3 objmaxstart {startpos + hitboxmax};
        vec3 broadmin {glm::min(objminstart, objminstart + vel) - vec3{1.0f}};
        vec3 broadmax {glm::max(objmaxstart, objmaxstart + vel) + vec3{1.0f}};
        vec3 blocksmin {floor(broadmin)};
        vec3 blocksmax {ceil(broadmax)};
        for (int i = 0; i < 3; i++) {
            if (rvel == vec3{0.0f}) break;
            vec3 curcolnor {0.0f};
            float curcoltime = 1.0f;
            bool collided = false;
            for (int x = blocksmin.x; x <= blocksmax.x; x++) {
                for (int y = blocksmin.y; y <= blocksmax.y; y++) {
                    for (int z = blocksmin.z; z <= blocksmax.z; z++) {
                        ivec3 blockpos {x, y, z};
                        if (chunksys.getblock(blockpos) == 0) continue;
                        vec3 blockmin {blockpos};
                        vec3 blockmax {blockmin + vec3{1.0f}};
                        vec3 colnor;
                        float coltime;
                        if (aabbinters(objminstart, objmaxstart, blockmin, blockmax, rvel, colnor, coltime)) {
                            if (!collided || coltime < curcoltime) {
                                curcoltime = coltime;
                                curcolnor = colnor;
                                collided = true;
                            }
                        }
                    }
                }
            }
            if (collided) {
                startpos += rvel * curcoltime + curcolnor * 0.001f;
                rvel *= 1.0f - curcoltime;
                if (curcolnor.x != 0.0f) {
                    rvel.x = 0.0f;
                    colres.x = true;
                } else if (curcolnor.y != 0.0f) {
                    rvel.y = 0.0f;
                    if (curcolnor.y > 0.0f) {
                        colres.floor = true;
                    }
                    else colres.ceil = true;
                } else if (curcolnor.z != 0.0f) {
                    rvel.z = 0.0f;
                    colres.z = true;
                }
                objminstart = startpos + hitboxmin;
                objmaxstart = startpos + hitboxmax;
            } else {
                startpos += rvel;
                break;
            }
        }
        pos = startpos;
        return colres;
    }
};