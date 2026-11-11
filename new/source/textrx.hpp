#pragma once

#include <font/sefont.h>
#include <config.hpp>
#include <shader.hpp>

Shader textrxshd;

void crtextrxshd() {
    const string vertcode = R"(
        #version 330 core
        layout (location = 0) in vec2 pos;
        layout (location = 1) in vec4 color;
        out vec4 newcolor;
        void main() {
            gl_Position = vec4(pos, 0.0, 1.0);
            newcolor = color;
        }
    )";
    const string fragcode = R"(
        #version 330 core
        in vec4 newcolor;
        out vec4 FragColor;
        void main() {
            FragColor = newcolor;
        }
    )";
    textrxshd.crself(vertcode, fragcode);
}

struct Tvert {
    vec2 pos;
    vec4 color;
};

struct Textrx {
    GLuint vao = 0;
    GLuint vbo = 0;
    GLuint ebo = 0;

    vector<Tvert> verts;
    vector<uint32> indxs;

    void render(const string &text, const vec4 &color, float scale, float x, float y) {
        verts.clear();
        indxs.clear();
        if (text.empty()) return;
        static char buffer[100000];
        char *textcstr = const_cast<char*>(text.c_str());
        scale *= min((float)scrw / 1200.0f, (float)scrh / 800.0f);
        int nquads = stb_easy_font_print(x / scale, y / scale, textcstr, nullptr, buffer, sizeof(buffer));
        float *v = reinterpret_cast<float*>(buffer);
        for (int i = 0; i < nquads; i++) {
			int offset = verts.size();
			vec2 v1 {vec2{((v[0]) * scale) / (float)(scrw) * 2.0f - 1.0f, 1.0f - ((v[1]) * scale) / (float)(scrh) * 2.0f}}; v += 4;
			vec2 v2 {vec2{((v[0]) * scale) / (float)(scrw) * 2.0f - 1.0f, 1.0f - ((v[1]) * scale) / (float)(scrh) * 2.0f}}; v += 4;
			vec2 v3 {vec2{((v[0]) * scale) / (float)(scrw) * 2.0f - 1.0f, 1.0f - ((v[1]) * scale) / (float)(scrh) * 2.0f}}; v += 4;
			vec2 v4 {vec2{((v[0]) * scale) / (float)(scrw) * 2.0f - 1.0f, 1.0f - ((v[1]) * scale) / (float)(scrh) * 2.0f}}; v += 4;
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
        textrxshd.use();
        glLineWidth(1.0f);
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(Tvert), verts.data(), GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indxs.size() * sizeof(uint32), indxs.data(), GL_DYNAMIC_DRAW);
        glDisable(GL_DEPTH_TEST);
        glDrawElements(GL_TRIANGLES, indxs.size(), GL_UNSIGNED_INT, 0);
        glEnable(GL_DEPTH_TEST);
    }

    void crself() {
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

    void delself() {
        glDeleteVertexArrays(1, &vao);
        glDeleteBuffers(1, &vbo);
        glDeleteBuffers(1, &ebo);
    }
};