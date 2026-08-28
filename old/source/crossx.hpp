#pragma once
#include <string>
#include <vector>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <config.hpp>
#include <shader.hpp>

Shader crossxshader;

void createcrossxshader() {
    const std::string vertcode = R"(
        #version 330 core
        layout (location = 0) in vec2 pos;
        uniform float whratio;
        void main() {
            gl_Position = vec4(pos.x / whratio, pos.y, 0.0, 1.0);
        }
    )";
    const std::string fragcode = R"(
        #version 330 core
        out vec4 FragColor;
        void main() {
            FragColor = vec4(1.0, 1.0, 1.0, 1.0);
        }
    )";
    crossxshader.createself(vertcode, fragcode);
}

struct Crossx {
    uint32_t vao = 0;
    uint32_t vbo = 0;

    float size = 0.015f;

    const std::vector<glm::vec2> verts = {
        {-size, -size}, { size, -size},
		{ size, -size}, { size,  size},
		{ size,  size}, {-size,  size},
		{-size,  size}, {-size, -size}
    };

    void createself() {
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(glm::vec2), verts.data(), GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);
    }

    void deleteself() {
        glDeleteVertexArrays(1, &vao);
        glDeleteBuffers(1, &vbo);
    }

    void drawself() {
        glLineWidth(2.0f);
        crossxshader.use();
        float whratio = (float)screenwidth / (float)screenheight;
        crossxshader.setfloat("whratio", whratio);
        glBindVertexArray(vao);
        glDisable(GL_DEPTH_TEST);
        glDrawArrays(GL_LINES, 0, verts.size());
        glEnable(GL_DEPTH_TEST);
    }
};