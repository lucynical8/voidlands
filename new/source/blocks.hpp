#pragma once

#include <imgl/simage.h>
#include <config.hpp>
#include <bmeshx.hpp>

GLuint blocktexarr = 0;
vector<Bmesh> blockmeshes;

const vector<string> blocknames {
    "stone", "tuff", "lumm", "ice",
    "magma", "amethyst", "emerald"
};

const vector<string> texpaths {
    "../resources/textures/stone.png",
    "../resources/textures/tuff.png",
    "../resources/textures/lumm.png",
    "../resources/textures/ice.png",
    "../resources/textures/magma.png",
    "../resources/textures/amethyst.png",
    "../resources/textures/emerald.png"
};

const vector<vector<int>> texfaces {
    {0, 0, 0, 0, 0, 0},
    {1, 1, 1, 1, 1, 1},
    {2, 2, 2, 2, 2, 2},
    {3, 3, 3, 3, 3, 3},
    {4, 4, 4, 4, 4, 4},
    {5, 5, 5, 5, 5, 5},
    {6, 6, 6, 6, 6, 6}
};

const vector<ivec3> stdfaces {
    {0, 0, 1}, {0, 0, -1},
    {-1, 0, 0}, {1, 0, 0},
    {0, 1, 0}, {0, -1, 0}
};

const vector<Fvert> stdverts {
    {{0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f},  {0.0f, 0.0f}, 0.0f, 1.0f},
    {{0.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 1.0f},  {0.0f, 1.0f}, 0.0f, 1.0f},
    {{1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 1.0f},  {1.0f, 1.0f}, 0.0f, 1.0f},
    {{1.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f},  {1.0f, 0.0f}, 0.0f, 1.0f},
    {{1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f}, 0.0f, 1.0f},
    {{1.0f, 1.0f, 0.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f}, 0.0f, 1.0f},
    {{0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, -1.0f}, {1.0f, 1.0f}, 0.0f, 1.0f},
    {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, -1.0f}, {1.0f, 0.0f}, 0.0f, 1.0f},
    {{0.0f, 0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}, 0.0f, 1.0f},
    {{0.0f, 1.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}, 0.0f, 1.0f},
    {{0.0f, 1.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}, 0.0f, 1.0f},
    {{0.0f, 0.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}, 0.0f, 1.0f},
    {{1.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 0.0f},  {0.0f, 0.0f}, 0.0f, 1.0f},
    {{1.0f, 1.0f, 1.0f}, {1.0f, 0.0f, 0.0f},  {0.0f, 1.0f}, 0.0f, 1.0f},
    {{1.0f, 1.0f, 0.0f}, {1.0f, 0.0f, 0.0f},  {1.0f, 1.0f}, 0.0f, 1.0f},
    {{1.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f},  {1.0f, 0.0f}, 0.0f, 1.0f},
    {{0.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 0.0f},  {0.0f, 0.0f}, 0.0f, 1.0f},
    {{0.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f},  {0.0f, 1.0f}, 0.0f, 1.0f},
    {{1.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f},  {1.0f, 1.0f}, 0.0f, 1.0f},
    {{1.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 0.0f},  {1.0f, 0.0f}, 0.0f, 1.0f},
    {{0.0f, 0.0f, 0.0f}, {0.0f, -1.0f, 0.0f}, {0.0f, 0.0f}, 0.0f, 1.0f},
    {{0.0f, 0.0f, 1.0f}, {0.0f, -1.0f, 0.0f}, {0.0f, 1.0f}, 0.0f, 1.0f},
    {{1.0f, 0.0f, 1.0f}, {0.0f, -1.0f, 0.0f}, {1.0f, 1.0f}, 0.0f, 1.0f},
    {{1.0f, 0.0f, 0.0f}, {0.0f, -1.0f, 0.0f}, {1.0f, 0.0f}, 0.0f, 1.0f}
};

const vector<uint32> stdindxs {
     0,  2,  1,  0,  3,  2,
     4,  6,  5,  4,  7,  6,
     8, 10,  9,  8, 11, 10,
    12, 14, 13, 12, 15, 14,
    16, 18, 17, 16, 19, 18,
    20, 22, 21, 20, 23, 22
};

struct Blocksys {
    void loadtexs() {
        int width = 16;
        int height = 16;
        int nchanns = 4;
        int layers = texpaths.size();
        glGenTextures(1, &blocktexarr);
        glBindTexture(GL_TEXTURE_2D_ARRAY, blocktexarr);
        glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA8, width, height, layers, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
        stbi_set_flip_vertically_on_load(true);
        for (int i = 0; i < layers; i++) {
            unsigned char *data = stbi_load(texpaths[i].c_str(), &width, &height, &nchanns, 4);
            glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, i, width, height, 1, GL_RGBA, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTextureParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTextureParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
    }

    void crmeshes() {
        Bmesh tempmesh;
        for (int i = 0; i < texfaces.size(); i++) {
            tempmesh.verts = stdverts;
            tempmesh.indxs = stdindxs;
            for (int face = 0; face < 6; face++) {
                for (int vert = 0; vert < 4; vert++) {
                    tempmesh.verts[face * 4 + vert].texid = texfaces[i][face];
                }
            }
            blockmeshes.push_back(tempmesh);
        }
    }
};