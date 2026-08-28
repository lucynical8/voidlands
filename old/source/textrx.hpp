#pragma once
#include <string>
#include <vector>
#include <algorithm>
#include <fontloader/stb_easy_font.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <config.hpp>
#include <shader.hpp>

Shader textrxshader;

void createtextrxshader() {
    const std::string vertcode = R"(
        #version 330 core
        layout (location = 0) in vec2 pos;
        layout (location = 1) in vec4 color;
        out vec4 newcolor;
        void main() {
            gl_Position = vec4(pos, 0.0, 1.0);
            newcolor = color;
        }
    )";
    const std::string fragcode = R"(
        #version 330 core
        in vec4 newcolor;
        out vec4 FragColor;
        void main() {
            FragColor = newcolor;
        }
    )";
    textrxshader.createself(vertcode, fragcode);
}

struct Tvert {
    glm::vec2 pos;
    glm::vec4 color;
};

struct Textrx {
    uint32_t vao = 0;
    uint32_t vbo = 0;
    uint32_t ebo = 0;

    std::vector<Tvert> verts;
    std::vector<uint32_t> indxs;

    void render(const std::string &text, const glm::vec4 &color, float scale, float x, float y) {
        verts.clear();
        indxs.clear();
        if (text.empty()) return;
        static char buffer[100000];
        char *textcstr = const_cast<char*>(text.c_str());
        scale *= std::min((float)screenwidth / 1200.0f, (float)screenheight / 800.0f);
        int nquads = stb_easy_font_print(x / scale, y / scale, textcstr, nullptr, buffer, sizeof(buffer));
        float *v = reinterpret_cast<float*>(buffer);
        for (int i = 0; i < nquads; i++) {
			int offset = verts.size();
			glm::vec2 v1 = glm::vec2(
				((v[0]) * scale) / (float)(screenwidth) * 2.0f - 1.0f,
				1.0f - ((v[1]) * scale) / (float)(screenheight) * 2.0f
			);
			v += 4;
			glm::vec2 v2 = glm::vec2(
				((v[0]) * scale) / (float)(screenwidth) * 2.0f - 1.0f,
				1.0f - ((v[1]) * scale) / (float)(screenheight) * 2.0f
			);
			v += 4;
			glm::vec2 v3 = glm::vec2(
				((v[0]) * scale) / (float)(screenwidth) * 2.0f - 1.0f,
				1.0f - ((v[1]) * scale) / (float)(screenheight) * 2.0f
			);
			v += 4;
			glm::vec2 v4 = glm::vec2(
				((v[0]) * scale) / (float)(screenwidth) * 2.0f - 1.0f,
				1.0f - ((v[1]) * scale) / (float)(screenheight) * 2.0f
			);
			v += 4;
			verts.push_back({v1, color});
			verts.push_back({v2, color});
			verts.push_back({v3, color});
			verts.push_back({v4, color});
			indxs.push_back(offset + 0);
			indxs.push_back(offset + 2);
			indxs.push_back(offset + 1);
			indxs.push_back(offset + 0);
			indxs.push_back(offset + 3);
			indxs.push_back(offset + 2);
		}
        textrxshader.use();
        glLineWidth(1.0f);
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(Tvert), verts.data(), GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indxs.size() * sizeof(uint32_t), indxs.data(), GL_DYNAMIC_DRAW);
        glDisable(GL_DEPTH_TEST);
        glDrawElements(GL_TRIANGLES, indxs.size(), GL_UNSIGNED_INT, 0);
        glEnable(GL_DEPTH_TEST);
    }

    void createself() {
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        glGenBuffers(1, &ebo);
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Tvert), (void*)offsetof(Tvert, pos));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Tvert), (void*)offsetof(Tvert, color));
    }

    void deleteself() {
        glDeleteVertexArrays(1, &vao);
        glDeleteBuffers(1, &vbo);
        glDeleteBuffers(1, &ebo);
    }
};