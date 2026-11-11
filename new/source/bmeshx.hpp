#pragma once

#include <config.hpp>
#include <shader.hpp>

Shader bmeshshd;

void crbmeshshd() {
    const string vertcode = R"(
        #version 330 core
        layout (location = 0) in vec3 pos;
        layout (location = 1) in vec3 nor;
        layout (location = 2) in vec2 uvs;
        layout (location = 3) in float texid;
        layout (location = 4) in float light;
        out vec2 newuvs;
        out float ntexid;
        out float nlight;
        uniform mat4 model;
        uniform mat4 view;
        uniform mat4 proj;
        void main() {
            newuvs = uvs;
            ntexid = texid;
            nlight = light;
            gl_Position = proj * view * model * vec4(pos, 1.0);
        }
    )";
    const string fragcode = R"(
        #version 330 core
        in vec2 newuvs;
        in float ntexid;
        in float nlight;
        out vec4 FragColor;
        uniform sampler2DArray blocktexarr;
        void main() {
            vec4 texcolor = texture(blocktexarr, vec3(newuvs, ntexid));
            FragColor = vec4(texcolor.rgb * clamp(nlight, 0.05, 1.0), texcolor.a);
        }
    )";
    bmeshshd.crself(vertcode, fragcode);
}

struct Fvert {
    vec3 pos;
    vec3 nor;
    vec2 uvs;
    float texid;
    float light;
};

struct Bmesh {
    GLuint vao = 0;
    GLuint vbo = 0;
    GLuint ebo = 0;

    int indxcnt = 0;

    vector<Fvert> verts;
    vector<uint32> indxs;

    void crself() {
        if (!vao) genself();
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(Fvert), verts.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indxs.size() * sizeof(uint32), indxs.data(), GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Fvert), (void*)offsetof(Fvert, pos));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Fvert), (void*)offsetof(Fvert, nor));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Fvert), (void*)offsetof(Fvert, uvs));
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Fvert), (void*)offsetof(Fvert, texid));
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, sizeof(Fvert), (void*)offsetof(Fvert, light));
        indxcnt = indxs.size();
        remself();
    }

    void genself() {
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        glGenBuffers(1, &ebo);
    }

    void delself() {
        glDeleteVertexArrays(1, &vao);
        glDeleteBuffers(1, &vbo);
        glDeleteBuffers(1, &ebo);
    }

    void remself() {
        verts.clear();
        indxs.clear();
        verts.shrink_to_fit();
        indxs.shrink_to_fit();
    }
};