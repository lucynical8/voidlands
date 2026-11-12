#pragma once

#include <config.hpp>

struct Shader {
    GLuint id = 0;

    void use() const {
        glUseProgram(id);
    }

    void crself(const string &vertcode, const string &fragcode) {
        GLuint vert = 0;
        GLuint frag = 0;
        crvert(vert, vertcode);
        crfrag(frag, fragcode);
        id = glCreateProgram();
        glAttachShader(id, vert);
        glAttachShader(id, frag);
        glDeleteShader(vert);
        glDeleteShader(frag);
        glLinkProgram(id);
    }

    void crvert(GLuint &vert, const string &code) {
        const char *codecstr = code.c_str();
        vert = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vert, 1, &codecstr, NULL);
        glCompileShader(vert);
    }

    void crfrag(GLuint &frag, const string &code) {
        const char *codecstr = code.c_str();
        frag = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(frag, 1, &codecstr, NULL);
        glCompileShader(frag);
    }

    void setint(const string &name, int value) const {
        glUniform1i(glGetUniformLocation(id, name.c_str()), value);
    }

    void setfloat(const string &name, float value) const {
        glUniform1f(glGetUniformLocation(id, name.c_str()), value);
    }

    void setvec2(const string &name, const vec2 &vec) const {
        glUniform2fv(glGetUniformLocation(id, name.c_str()), 1, &vec[0]);
    }

    void setvec3(const string &name, const vec3 &vec) const {
        glUniform3fv(glGetUniformLocation(id, name.c_str()), 1, &vec[0]);
    }

    void setvec4(const string &name, const vec4 &vec) const {
        glUniform4fv(glGetUniformLocation(id, name.c_str()), 1, &vec[0]);
    }

    void setmat4(const string &name, const mat4 &mat) const {
        glUniformMatrix4fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
};