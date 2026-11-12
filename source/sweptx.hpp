#pragma once

#include <config.hpp>
#include <chunks.hpp>

using glm::ceil;
using glm::floor;
using glm::length;

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
            if (vel[i] >= 0.0f) {
                tentry[i] = (bmin[i] - amax[i]) / (vel[i] != 0.0f ? vel[i] : 0.0001f);
                texit[i]  = (bmax[i] - amin[i]) / (vel[i] != 0.0f ? vel[i] : 0.0001f);
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

    void resolblocks(vec3 &pos, vec3 &vel, const vec3 &hitboxmin, const vec3 &hitboxmax, Chunksys &chunksys) {
        if (vel == vec3{0.0f}) return;
        vec3 rvel {vel};
        vec3 startpos {pos};
        vec3 objminstart {startpos + hitboxmin};
        vec3 objmaxstart {startpos + hitboxmax};
        vec3 broadmin {glm::min(objminstart, objminstart + vel) - vec3{1.0f}};
        vec3 broadmax {glm::max(objmaxstart, objmaxstart + vel) + vec3{1.0f}};
        vec3 blocksmin {floor(broadmin)};
        vec3 blocksmax {ceil(broadmax)};
        for (int i = 0; i < 3; i++) {
            if (length(rvel) < 0.0001f) break;
            vec3 curcolnor {0.0f};
            float curcoltime = 1.0f;
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
                            if (coltime < curcoltime) {
                                curcoltime = coltime;
                                curcolnor = colnor;
                            }
                        }
                    }
                }
            }
            if (curcoltime < 1.0f) {
                startpos += curcolnor * 0.001f;
                if (curcolnor.x != 0.0f) {
                    startpos.x += rvel.x * curcoltime;
                    rvel.x = 0.0f;
                } else if (curcolnor.y != 0.0f) {
                    startpos.y += rvel.y * curcoltime;
                    rvel.y = 0.0f;
                } else if (curcolnor.z != 0.0f) {
                    startpos.z += rvel.z * curcoltime;
                    rvel.z = 0.0f;
                }
                objminstart = startpos + hitboxmin;
                objmaxstart = startpos + hitboxmax;
            } else {
                startpos += rvel;
                break;
            }
        }
        pos = startpos;
    }
};