#pragma once
#include <string>
#include <vector>
#include <imgloader/stb_image.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <bmeshx.hpp>

uint32_t blocktexarray;

std::vector<Bmesh> blockmeshes;

const std::vector<std::string> blocknames = {
    "stone",
    "tuff",
    "lumm",
    "ice",
    "magma",
    "amethyst",
    "emerald"
};

const std::vector<glm::ivec3> stdblockfaces = {
    {0, 0, 1}, {0, 0, -1},
    {-1, 0, 0}, {1, 0, 0},
    {0, 1, 0}, {0, -1, 0}
};

struct Blocksystem {
    const std::vector<std::string> texpaths = {
        "../resources/textures/stone.png",
        "../resources/textures/tuff.png",
        "../resources/textures/lumm.png",
        "../resources/textures/ice.png",
        "../resources/textures/magma.png",
        "../resources/textures/amethyst.png",
        "../resources/textures/emerald.png"
    };

    const std::vector<std::vector<int>> texfaces = {
        {0, 0, 0, 0, 0, 0},
        {1, 1, 1, 1, 1, 1},
        {2, 2, 2, 2, 2, 2},
        {3, 3, 3, 3, 3, 3},
        {4, 4, 4, 4, 4, 4},
        {5, 5, 5, 5, 5, 5},
        {6, 6, 6, 6, 6, 6}
    };

    void loadtextures() {
        int width = 16;
        int height = 16;
        int nchannels = 4;
        int layers = texpaths.size();
        stbi_set_flip_vertically_on_load(true);
        glGenTextures(1, &blocktexarray);
        glBindTexture(GL_TEXTURE_2D_ARRAY, blocktexarray);
        glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA8, width, height, layers, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
        for (int i = 0; i < layers; i++) {
            unsigned char *data = stbi_load(texpaths[i].c_str(), &width, &height, &nchannels, 4);
            glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, i, width, height, 1, GL_RGBA, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTextureParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTextureParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
    }

    void createmeshes() {
        for (int i = 0; i < texfaces.size(); i++) {
            Bmesh mesh;
            mesh.verts = stdverts;
            mesh.indxs = stdindxs;
            for (int face = 0; face < 6; face++) {
                for (int vert = 0; vert < 4; vert++) {
                    mesh.verts[face * 4 + vert].texid = texfaces[i][face];
                }
            }
            blockmeshes.push_back(mesh);
        }
    }

    const std::vector<Fvert> stdverts = {
        {{0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}, 0.0f, 1.0f},
        {{0.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}, 0.0f, 1.0f},
        {{1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}, 0.0f, 1.0f},
        {{1.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}, 0.0f, 1.0f},
        {{1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f}, 0.0f, 1.0f},
        {{1.0f, 1.0f, 0.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f}, 0.0f, 1.0f},
        {{0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, -1.0f}, {1.0f, 1.0f}, 0.0f, 1.0f},
        {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, -1.0f}, {1.0f, 0.0f}, 0.0f, 1.0f},
        {{0.0f, 0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}, 0.0f, 1.0f},
        {{0.0f, 1.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}, 0.0f, 1.0f},
        {{0.0f, 1.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}, 0.0f, 1.0f},
        {{0.0f, 0.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}, 0.0f, 1.0f},
        {{1.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}, 0.0f, 1.0f},
        {{1.0f, 1.0f, 1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}, 0.0f, 1.0f},
        {{1.0f, 1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}, 0.0f, 1.0f},
        {{1.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}, 0.0f, 1.0f},
        {{0.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}, 0.0f, 1.0f},
        {{0.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}, 0.0f, 1.0f},
        {{1.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}, 0.0f, 1.0f},
        {{1.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}, 0.0f, 1.0f},
        {{0.0f, 0.0f, 0.0f}, {0.0f, -1.0f, 0.0f}, {0.0f, 0.0f}, 0.0f, 1.0f},
        {{0.0f, 0.0f, 1.0f}, {0.0f, -1.0f, 0.0f}, {0.0f, 1.0f}, 0.0f, 1.0f},
        {{1.0f, 0.0f, 1.0f}, {0.0f, -1.0f, 0.0f}, {1.0f, 1.0f}, 0.0f, 1.0f},
        {{1.0f, 0.0f, 0.0f}, {0.0f, -1.0f, 0.0f}, {1.0f, 0.0f}, 0.0f, 1.0f}
    };

    const std::vector<uint32_t> stdindxs = {
        0, 2, 1, 0, 3, 2,
        4, 6, 5, 4, 7, 6,
        8, 10, 9, 8, 11, 10,
        12, 14, 13, 12, 15, 14,
        16, 18, 17, 16, 19, 18,
        20, 22, 21, 20, 23, 22
    };
};