#pragma once

#include <queue>
#include <config.hpp>
#include <blocks.hpp>

struct Light {
    ivec3 pos;
    int indx;
};

struct Lummis {
    void genlights(array<uint8, 4096> (&lights)[27], const array<uint8, 4096> (&blocks)[27]) {
        int reltoidx[3][3][3];
        ivec3 relcoords[27];
        for (int i = 0; i < 27; i++) {
            fill(lights[i].begin(), lights[i].end(), 0);
            ivec3 relpos {i/9%3-1, i/3%3-1, i/1%3-1};
            reltoidx[relpos.x + 1][relpos.y + 1][relpos.z + 1] = i;
            relcoords[i] = relpos;
        }
        queue<Light> lightque;
        for (int i = 0; i < 27; i++) {
            for (int x = 0; x < 16; x++) {
                for (int y = 0; y < 16; y++) {
                    for (int z = 0; z < 16; z++) {
                        if (blocks[i][blockindx({x, y, z})] == 3) {
                            lights[i][blockindx({x, y, z})] = 15;
                            lightque.push({{x, y, z}, i});
                        }
                        if (blocks[i][blockindx({x, y, z})] == 5) {
                            lights[i][blockindx({x, y, z})] = 7;
                            lightque.push({{x, y, z}, i});
                        }
                        if (blocks[i][blockindx({x, y, z})] == 6) {
                            lights[i][blockindx({x, y, z})] = 5;
                            lightque.push({{x, y, z}, i});
                        }
                    }
                }
            }
        }
        while (!lightque.empty()) {
            Light light {lightque.front()};
            uint8 clight = lights[light.indx][blockindx(light.pos)];
            if (clight <= 1) goto skip;
            for (int d = 0; d < 6; d++) {
                ivec3 delta {stdfaces[d]};
                ivec3 relpos {relcoords[light.indx]};
                ivec3 npos {light.pos + delta};
                bool valid = true;
                for (int i = 0; i < 3; i++) {
                    if (npos[i] < 0) {
                        relpos[i] -= 1;
                        npos[i] = 15;
                    } else if (npos[i] >= 16) {
                        relpos[i] += 1;
                        npos[i] = 0;
                    }
                    if (relpos[i] < -1 || relpos[i] > 1) {
                        valid = false;
                        break;
                    }
                }
                if (!valid) continue;
                int ni = reltoidx[relpos.x + 1][relpos.y + 1][relpos.z + 1];
                if (blocks[ni][blockindx(npos)] != 0) continue;
                uint8 nlight = clight - 1;
                if (lights[ni][blockindx(npos)] >= nlight) continue;
                lights[ni][blockindx(npos)] = nlight;
                lightque.push({npos, ni});
            }
            skip: lightque.pop();
        }
    }

    int blockindx(const ivec3 &pos) {
        return pos.x * 256 + pos.y * 16 + pos.z;
    }
};