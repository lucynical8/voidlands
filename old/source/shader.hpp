#pragma once
#include <string>
#include <glad/glad.h>
#include <glm/glm.hpp>

struct Shader {
    uint32_t id = 0;

    void use() {
        glUseProgram(id);
    }

    void createself(const std::string &vertcode, const std::string &fragcode) {
        uint32_t vert = 0;
        uint32_t frag = 0;
        createvertshader(vert, vertcode.c_str());
        createfragshader(frag, fragcode.c_str());
        id = glCreateProgram();
        glAttachShader(id, vert);
        glAttachShader(id, frag);
        glLinkProgram(id);
        glDeleteShader(vert);
        glDeleteShader(frag);
    }

    void createvertshader(uint32_t &vert, const char *code) {
        vert = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vert, 1, &code, NULL);
        glCompileShader(vert);
    }

    void createfragshader(uint32_t &frag, const char *code) {
        frag = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(frag, 1, &code, NULL);
        glCompileShader(frag);
    }

    void setint(const std::string &name, int value) const {
        glUniform1i(glGetUniformLocation(id, name.c_str()), value);
    }

    void setfloat(const std::string &name, float value) const {
        glUniform1f(glGetUniformLocation(id, name.c_str()), value);
    }

    void setvec2(const std::string &name, const glm::vec2 &vec) const {
        glUniform2fv(glGetUniformLocation(id, name.c_str()), 1, &vec[0]);
    }

    void setvec3(const std::string &name, const glm::vec3 &vec) const {
        glUniform3fv(glGetUniformLocation(id, name.c_str()), 1, &vec[0]);
    }

    void setvec4(const std::string &name, const glm::vec4 &vec) const {
        glUniform4fv(glGetUniformLocation(id, name.c_str()), 1, &vec[0]);
    }

    void setmat2(const std::string &name, const glm::mat2 &mat) const {
        glUniformMatrix2fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

    void setmat3(const std::string &name, const glm::mat3 &mat) const {
        glUniformMatrix3fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

    void setmat4(const std::string &name, const glm::mat4 &mat) const {
        glUniformMatrix4fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
};