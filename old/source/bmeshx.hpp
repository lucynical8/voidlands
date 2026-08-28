#pragma once
#include <string>
#include <vector>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <shader.hpp>

Shader bmeshshader;

void createbmeshshader() {
    const std::string vertcode = R"(
        #version 330 core
        layout (location = 0) in vec3 pos;
        layout (location = 1) in vec3 nor;
        layout (location = 2) in vec2 uv;
        layout (location = 3) in float texid;
        layout (location = 4) in float light;
        out vec2 newuv;
        out float newtexid;
        out float newlight;
        uniform mat4 model;
        uniform mat4 view;
        uniform mat4 proj;
        void main() {
            newuv = uv;
            newtexid = texid;
            newlight = light;
            gl_Position = proj * view * model * vec4(pos, 1.0);
        }
    )";
    const std::string fragcode = R"(
        #version 330 core
        in vec2 newuv;
        in float newtexid;
        in float newlight;
        out vec4 FragColor;
        uniform sampler2DArray blocktexarray;
        void main() {
            vec4 texcolor = texture(blocktexarray, vec3(newuv, newtexid));
            FragColor = vec4(texcolor.rgb * clamp(newlight, 0.05, 1.0), texcolor.a);
        }
    )";
    bmeshshader.createself(vertcode, fragcode);
}

struct Fvert {
    glm::vec3 pos;
    glm::vec3 nor;
    glm::vec2 uv;
    float texid;
    float light;
};

struct Bmesh {
    uint32_t vao;
    uint32_t vbo;
    uint32_t ebo;

    int indxscount;

    std::vector<Fvert> verts;
    std::vector<uint32_t> indxs;

    void createself() {
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        glGenBuffers(1, &ebo);
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(Fvert), verts.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indxs.size() * sizeof(uint32_t), indxs.data(), GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Fvert), (void*)offsetof(Fvert, pos));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Fvert), (void*)offsetof(Fvert, nor));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Fvert), (void*)offsetof(Fvert, uv));
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Fvert), (void*)offsetof(Fvert, texid));
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, sizeof(Fvert), (void*)offsetof(Fvert, light));
        indxscount = indxs.size();
        verts.clear();
        indxs.clear();
        verts.shrink_to_fit();
        indxs.shrink_to_fit();
    }

    void deleteself() {
        glDeleteVertexArrays(1, &vao);
        glDeleteBuffers(1, &vbo);
        glDeleteBuffers(1, &ebo);
    }
};