#pragma once

#include <config.hpp>
#include <shader.hpp>

Shader crossxshd;

void crcrossxshd() {
    const string vertcode = R"(
        #version 330 core
        layout (location = 0) in vec2 pos;
        uniform float whratio;
        void main() {
            gl_Position = vec4(pos.x / whratio, pos.y, 0.0, 1.0);
        }
    )";
    const string fragcode = R"(
        #version 330 core
        out vec4 FragColor;
        void main() {
            FragColor = vec4(1.0, 1.0, 1.0, 1.0);
        }
    )";
    crossxshd.crself(vertcode, fragcode);
}

struct Crossx {
    GLuint vao = 0;
    GLuint vbo = 0;

    float size = 0.015f;

    const vector<vec2> verts {
        {-size, -size}, { size, -size},
		{ size, -size}, { size,  size},
		{ size,  size}, {-size,  size},
		{-size,  size}, {-size, -size}
    };

    void crself() {
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(vec2), verts.data(), GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(vec2), (void*)0);
    }

    void delself() {
        glDeleteVertexArrays(1, &vao);
        glDeleteBuffers(1, &vbo);
    }

    void drawself() {
        crossxshd.use();
        float whratio = (float)scrw / (float)scrh;
        crossxshd.setfloat("whratio", whratio);
        glLineWidth(2.0f);
        glBindVertexArray(vao);
        glDisable(GL_DEPTH_TEST);
        glDrawArrays(GL_LINES, 0, verts.size());
        glEnable(GL_DEPTH_TEST);
    }
};