#pragma once
#include <array>
#include <algorithm>
#include <glm/glm.hpp>
#include <bmeshx.hpp>
#include <blocks.hpp>

struct Greedy {
    void genmeshnative(Bmesh &mesh, const std::array<uint8_t, 4096> (&blocks)[27], const std::array<uint8_t, 4096> (&lights)[27]) {
        for (int x = 0; x < 16; x++) {
            for (int y = 0; y < 16; y++) {
                for (int z = 0; z < 16; z++) {
                    glm::ivec3 pos = glm::ivec3(x, y, z);
                    uint8_t block = blocks[13][blockindx(pos)];
                    if (block == 0) continue;
                    for (int face = 0; face < 6; face++) {
                        glm::ivec3 npos = pos + stdblockfaces[face];
                        uint8_t nblock = getblock(npos, blocks);
                        if (nblock != 0) continue;
                        uint8_t nlight = getlight(npos, lights);
                        const Bmesh &blockmesh = blockmeshes[block - 1];
                        int offset = mesh.verts.size();
                        for (int vert = 0; vert < 4; vert++) {
                            Fvert tempvert = blockmesh.verts[face * 4 + vert];
                            tempvert.pos += glm::vec3(pos);
                            tempvert.light = (float)nlight / 30.0f;
                            mesh.verts.push_back(tempvert);
                        }
                        for (int i = 0; i < 6; i++) {
                            mesh.indxs.push_back(offset + blockmesh.indxs[i]);
                        }
                    }
                }
            }
        }
    }

    void genmeshgreedy(Bmesh &mesh, const std::array<uint8_t, 4096> (&blocks)[27], const std::array<uint8_t, 4096> (&lights)[27]) {
        meshfacez(mesh, blocks, lights, 0);
        meshfacez(mesh, blocks, lights, 1);
        meshfacex(mesh, blocks, lights, 2);
        meshfacex(mesh, blocks, lights, 3);
        meshfacey(mesh, blocks, lights, 4);
        meshfacey(mesh, blocks, lights, 5);
    }

    void meshfacez(Bmesh &mesh, const std::array<uint8_t, 4096> (&blocks)[27], const std::array<uint8_t, 4096> (&lights)[27], int face) {
        for (int z = 0; z < 16; z++) {
            bool smask[16][16];
            uint8_t sblocks[16][16];
            uint8_t slights[16][16];
            std::fill(&smask[0][0], &smask[0][0] + 256, false);
            std::fill(&sblocks[0][0], &sblocks[0][0] + 256, 0);
            std::fill(&slights[0][0], &slights[0][0] + 256, 0);
            for (int x = 0; x < 16; x++) {
                for (int y = 0; y < 16; y++) {
                    glm::ivec3 pos = glm::ivec3(x, y, z);
                    uint8_t block = blocks[13][blockindx(pos)];
                    if (block == 0) continue;
                    glm::ivec3 npos = pos + stdblockfaces[face];
                    uint8_t nblock = getblock(npos, blocks);
                    if (nblock != 0) continue;
                    uint8_t nlight = getlight(npos, lights);
                    smask[x][y] = true;
                    sblocks[x][y] = block;
                    slights[x][y] = nlight;
                }
            }
            for (int x = 0; x < 16; x++) {
                for (int y = 0; y < 16; y++) {
                    if (!smask[x][y]) continue;
                    uint8_t block = sblocks[x][y];
                    uint8_t light = slights[x][y];
                    int wx = 1, h = 1;
                    while (x + wx < 16 && smask[x + wx][y] && sblocks[x + wx][y] == block && slights[x + wx][y] == light) {
                        wx++;
                    }
                    while (y + h < 16) {
                        for (int i = 0; i < wx; i++) {
                            if (!smask[x + i][y + h] || sblocks[x + i][y + h] != block || slights[x + i][y + h] != light) {
                                goto skip;
                            }
                        }
                        h++;
                    }
                    skip:;
                    for (int i = 0; i < wx; i++) {
                        for (int j = 0; j < h; j++) {
                            smask[x + i][y + j] = false;
                        }
                    }
                    createquad(mesh, face, x, y, z, wx, 1, h, block, light);
                }
            }
        }
    }

    void meshfacex(Bmesh &mesh, const std::array<uint8_t, 4096> (&blocks)[27], const std::array<uint8_t, 4096> (&lights)[27], int face) {
        for (int x = 0; x < 16; x++) {
            bool smask[16][16];
            uint8_t sblocks[16][16];
            uint8_t slights[16][16];
            std::fill(&smask[0][0], &smask[0][0] + 256, false);
            std::fill(&sblocks[0][0], &sblocks[0][0] + 256, 0);
            std::fill(&slights[0][0], &slights[0][0] + 256, 0);
            for (int z = 0; z < 16; z++) {
                for (int y = 0; y < 16; y++) {
                    glm::ivec3 pos = glm::ivec3(x, y, z);
                    uint8_t block = blocks[13][blockindx(pos)];
                    if (block == 0) continue;
                    glm::ivec3 npos = pos + stdblockfaces[face];
                    uint8_t nblock = getblock(npos, blocks);
                    if (nblock != 0) continue;
                    uint8_t nlight = getlight(npos, lights);
                    smask[z][y] = true;
                    sblocks[z][y] = block;
                    slights[z][y] = nlight;
                }
            }
            for (int z = 0; z < 16; z++) {
                for (int y = 0; y < 16; y++) {
                    if (!smask[z][y]) continue;
                    uint8_t block = sblocks[z][y];
                    uint8_t light = slights[z][y];
                    int wz = 1, h = 1;
                    while (z + wz < 16 && smask[z + wz][y] && sblocks[z + wz][y] == block && slights[z + wz][y] == light) {
                        wz++;
                    }
                    while (y + h < 16) {
                        for (int i = 0; i < wz; i++) {
                            if (!smask[z + i][y + h] || sblocks[z + i][y + h] != block || slights[z + i][y + h] != light) {
                                goto skip;
                            }
                        }
                        h++;
                    }
                    skip:;
                    for (int i = 0; i < wz; i++) {
                        for (int j = 0; j < h; j++) {
                            smask[z + i][y + j] = false;
                        }
                    }
                    createquad(mesh, face, x, y, z, 1, wz, h, block, light);
                }
            }
        }
    }

    void meshfacey(Bmesh &mesh, const std::array<uint8_t, 4096> (&blocks)[27], const std::array<uint8_t, 4096> (&lights)[27], int face) {
        for (int y = 0; y < 16; y++) {
            bool smask[16][16];
            uint8_t sblocks[16][16];
            uint8_t slights[16][16];
            std::fill(&smask[0][0], &smask[0][0] + 256, false);
            std::fill(&sblocks[0][0], &sblocks[0][0] + 256, 0);
            std::fill(&slights[0][0], &slights[0][0] + 256, 0);
            for (int x = 0; x < 16; x++) {
                for (int z = 0; z < 16; z++) {
                    glm::ivec3 pos = glm::ivec3(x, y, z);
                    uint8_t block = blocks[13][blockindx(pos)];
                    if (block == 0) continue;
                    glm::ivec3 npos = pos + stdblockfaces[face];
                    uint8_t nblock = getblock(npos, blocks);
                    if (nblock != 0) continue;
                    uint8_t nlight = getlight(npos, lights);
                    smask[x][z] = true;
                    sblocks[x][z] = block;
                    slights[x][z] = nlight;
                }
            }
            for (int x = 0; x < 16; x++) {
                for (int z = 0; z < 16; z++) {
                    if (!smask[x][z]) continue;
                    uint8_t block = sblocks[x][z];
                    uint8_t light = slights[x][z];
                    int wx = 1, wz = 1;
                    while (x + wx < 16 && smask[x + wx][z] && sblocks[x + wx][z] == block && slights[x + wx][z] == light) {
                        wx++;
                    }
                    while (z + wz < 16) {
                        for (int i = 0; i < wx; i++) {
                            if (!smask[x + i][z + wz] || sblocks[x + i][z + wz] != block || slights[x + i][z + wz] != light) {
                                goto skip;
                            }
                        }
                        wz++;
                    }
                    skip:;
                    for (int i = 0; i < wx; i++) {
                        for (int j = 0; j < wz; j++) {
                            smask[x + i][z + j] = false;
                        }
                    }
                    createquad(mesh, face, x, y, z, wx, wz, 1, block, light);
                }
            }
        }
    }

    void createquad(Bmesh &mesh, int face, int sx, int sy, int sz, int wx, int wz, int h, uint8_t block, uint8_t light) {
        const Bmesh &blockmesh = blockmeshes[block - 1];
        int offset = mesh.verts.size();
        glm::vec3 corners[4];
        glm::vec2 uvs[4];
        switch (face) {
            case 0:
                corners[0] = glm::vec3(sx, sy, sz + 1);
                corners[1] = glm::vec3(sx, sy + h, sz + 1);
                corners[2] = glm::vec3(sx + wx, sy + h, sz + 1);
                corners[3] = glm::vec3(sx + wx, sy, sz + 1);
                uvs[0] = glm::vec2(0.0f, 0.0f);
                uvs[1] = glm::vec2(0.0f, h);
                uvs[2] = glm::vec2(wx, h);
                uvs[3] = glm::vec2(wx, 0.0f);
                break;
            case 1:
                corners[0] = glm::vec3(sx + wx, sy, sz);
                corners[1] = glm::vec3(sx + wx, sy + h, sz);
                corners[2] = glm::vec3(sx, sy + h, sz);
                corners[3] = glm::vec3(sx, sy, sz);
                uvs[0] = glm::vec2(0.0f, 0.0f);
                uvs[1] = glm::vec2(0.0f, h);
                uvs[2] = glm::vec2(wx, h);
                uvs[3] = glm::vec2(wx, 0.0f);
                break;
            case 2:
                corners[0] = glm::vec3(sx, sy, sz);
                corners[1] = glm::vec3(sx, sy + h, sz);
                corners[2] = glm::vec3(sx, sy + h, sz + wz);
                corners[3] = glm::vec3(sx, sy, sz + wz);
                uvs[0] = glm::vec2(0.0f, 0.0f);
                uvs[1] = glm::vec2(0.0f, h);
                uvs[2] = glm::vec2(wz, h);
                uvs[3] = glm::vec2(wz, 0.0f);
                break;
            case 3:
                corners[0] = glm::vec3(sx + 1, sy, sz + wz);
                corners[1] = glm::vec3(sx + 1, sy + h, sz + wz);
                corners[2] = glm::vec3(sx + 1, sy + h, sz);
                corners[3] = glm::vec3(sx + 1, sy, sz);
                uvs[0] = glm::vec2(0.0f, 0.0f);
                uvs[1] = glm::vec2(0.0f, h);
                uvs[2] = glm::vec2(wz, h);
                uvs[3] = glm::vec2(wz, 0.0f);
                break;
            case 4:
                corners[0] = glm::vec3(sx, sy + 1, sz + wz);
                corners[1] = glm::vec3(sx, sy + 1, sz);
                corners[2] = glm::vec3(sx + wx, sy + 1, sz);
                corners[3] = glm::vec3(sx + wx, sy + 1, sz + wz);
                uvs[0] = glm::vec2(0.0f, 0.0f);
                uvs[1] = glm::vec2(0.0f, wz);
                uvs[2] = glm::vec2(wx, wz);
                uvs[3] = glm::vec2(wx, 0.0f);
                break;
            case 5:
                corners[0] = glm::vec3(sx, sy, sz);
                corners[1] = glm::vec3(sx, sy, sz + wz);
                corners[2] = glm::vec3(sx + wx, sy, sz + wz);
                corners[3] = glm::vec3(sx + wx, sy, sz);
                uvs[0] = glm::vec2(0.0f, 0.0f);
                uvs[1] = glm::vec2(0.0f, wz);
                uvs[2] = glm::vec2(wx, wz);
                uvs[3] = glm::vec2(wx, 0.0f);
                break;
        }
        for (int vert = 0; vert < 4; vert++) {
            Fvert tempvert = blockmesh.verts[face * 4 + vert];
            tempvert.pos = corners[vert];
            tempvert.uv = uvs[vert];
            tempvert.light = (float)light / 30.0f;
            mesh.verts.push_back(tempvert);
        }
        for (int i = 0; i < 6; i++) {
            mesh.indxs.push_back(offset + blockmesh.indxs[i]);
        }
    }

    uint8_t getblock(const glm::ivec3 &pos, const std::array<uint8_t, 4096> (&blocks)[27]) {
        if (pos.x < 0) return blocks[4][blockindx(glm::ivec3{15, pos.y, pos.z})];
        if (pos.x >= 16) return blocks[22][blockindx(glm::ivec3{0, pos.y, pos.z})];
        if (pos.y < 0) return blocks[10][blockindx(glm::ivec3{pos.x, 15, pos.z})];
        if (pos.y >= 16) return blocks[16][blockindx(glm::ivec3{pos.x, 0, pos.z})];
        if (pos.z < 0) return blocks[12][blockindx(glm::ivec3{pos.x, pos.y, 15})];
        if (pos.z >= 16) return blocks[14][blockindx(glm::ivec3{pos.x, pos.y, 0})];
        return blocks[13][blockindx(pos)];
    }

    uint8_t getlight(const glm::ivec3 &pos, const std::array<uint8_t, 4096> (&lights)[27]) {
        if (pos.x < 0) return lights[4][blockindx(glm::ivec3{15, pos.y, pos.z})];
        if (pos.x >= 16) return lights[22][blockindx(glm::ivec3{0, pos.y, pos.z})];
        if (pos.y < 0) return lights[10][blockindx(glm::ivec3{pos.x, 15, pos.z})];
        if (pos.y >= 16) return lights[16][blockindx(glm::ivec3{pos.x, 0, pos.z})];
        if (pos.z < 0) return lights[12][blockindx(glm::ivec3{pos.x, pos.y, 15})];
        if (pos.z >= 16) return lights[14][blockindx(glm::ivec3{pos.x, pos.y, 0})];
        return lights[13][blockindx(pos)];
    }

    int blockindx(const glm::ivec3 &pos) {
        return pos.x * 256 + pos.y * 16 + pos.z;
    }
};