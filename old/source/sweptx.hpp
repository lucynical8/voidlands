#pragma once
#include <algorithm>
#include <glm/glm.hpp>
#include <chunks.hpp>

struct Sweptx {
    bool aabbintersect(const glm::vec3 &amin,
                       const glm::vec3 &amax,
                       const glm::vec3 &bmin,
                       const glm::vec3 &bmax,
                       const glm::vec3 &vel,
                       glm::vec3 &nor,
                       float &coltime) {
        coltime = 1.0f;
        nor = glm::vec3(0.0f);
        if (vel == glm::vec3(0.0f)) {
            bool overlap = (amin.x < bmax.x && amax.x > bmin.x) &&
						   (amin.y < bmax.y && amax.y > bmin.y) &&
						   (amin.z < bmax.z && amax.z > bmin.z);
            if (!overlap) return false;
            glm::vec3 pen = glm::vec3(std::min(amax.x - bmin.x, bmax.x - amin.x),
                                      std::min(amax.y - bmin.y, bmax.y - amin.y),
                                      std::min(amax.z - bmin.z, bmax.z - amin.z));
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
        glm::vec3 tentry;
        glm::vec3 texit;
        for (int i = 0; i < 3; i++) {
            if (vel[i] >= 0.0f) {
                tentry[i] = (bmin[i] - amax[i]) / (vel[i] != 0.0f ? vel[i] : 0.0001f);
                texit[i]  = (bmax[i] - amin[i]) / (vel[i] != 0.0f ? vel[i] : 0.0001f);
            } else {
                tentry[i] = (bmax[i] - amin[i]) / vel[i];
                texit[i]  = (bmin[i] - amax[i]) / vel[i];
            }
        }
        float entrytime = std::max({tentry.x, tentry.y, tentry.z});
        float exittime  = std::min({texit.x, texit.y, texit.z});
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

    void resolveblocks(glm::vec3 &pos,
                       glm::vec3 &vel,
                       const glm::vec3 &hitboxmin,
                       const glm::vec3 &hitboxmax,
                       Chunksystem &chunksystem) {
        if (vel == glm::vec3(0.0f)) return;
        glm::vec3 rvel = vel;
        glm::vec3 startpos = pos;
        glm::vec3 objminstart = startpos + hitboxmin;
        glm::vec3 objmaxstart = startpos + hitboxmax;
        glm::vec3 broadmin = glm::min(objminstart, objminstart + vel) - glm::vec3(1.0f);
        glm::vec3 broadmax = glm::max(objmaxstart, objmaxstart + vel) + glm::vec3(1.0f);
        glm::vec3 blocksmin = glm::floor(broadmin);
        glm::vec3 blocksmax = glm::ceil(broadmax);
        for (int i = 0; i < 3; i++) {
            if (glm::length(rvel) < 0.0001f) break;
            glm::vec3 curcolnor = glm::vec3(0.0f);
            float curcoltime = 1.0f;
            for (int x = blocksmin.x; x <= blocksmax.x; x++) {
                for (int y = blocksmin.y; y <= blocksmax.y; y++) {
                    for (int z = blocksmin.z; z <= blocksmax.z; z++) {
                        glm::ivec3 blockpos = glm::ivec3(x, y, z);
                        if (chunksystem.getblock(blockpos) == 0) continue;
                        glm::vec3 blockmin = glm::vec3(blockpos);
                        glm::vec3 blockmax = blockmin + glm::vec3(1.0f);
                        glm::vec3 colnor;
                        float coltime;
                        if (aabbintersect(objminstart, objmaxstart, blockmin, blockmax, rvel, colnor, coltime)) {
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