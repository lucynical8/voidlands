#pragma once
#include <glm/glm.hpp>

struct Frustum {
    glm::vec4 planes[6];

    void update(const glm::mat4 &mat) {
        planes[0] = glm::vec4(
            mat[0][3] + mat[0][0],
            mat[1][3] + mat[1][0],
            mat[2][3] + mat[2][0],
            mat[3][3] + mat[3][0]);
        planes[1] = glm::vec4(
            mat[0][3] - mat[0][0],
            mat[1][3] - mat[1][0],
            mat[2][3] - mat[2][0],
            mat[3][3] - mat[3][0]);
        planes[2] = glm::vec4(
            mat[0][3] + mat[0][1],
            mat[1][3] + mat[1][1],
            mat[2][3] + mat[2][1],
            mat[3][3] + mat[3][1]);
        planes[3] = glm::vec4(
            mat[0][3] - mat[0][1],
            mat[1][3] - mat[1][1],
            mat[2][3] - mat[2][1],
            mat[3][3] - mat[3][1]);
        planes[4] = glm::vec4(
            mat[0][3] + mat[0][2],
            mat[1][3] + mat[1][2],
            mat[2][3] + mat[2][2],
            mat[3][3] + mat[3][2]);
        planes[5] = glm::vec4(
            mat[0][3] - mat[0][2],
            mat[1][3] - mat[1][2],
            mat[2][3] - mat[2][2],
            mat[3][3] - mat[3][2]);
        for (int i = 0; i < 6; i++) {
            float len = glm::length(glm::vec3(planes[i]));
            if (len <= 0.0f) continue;
            planes[i] /= len;
        }
    }

    bool isvisible(const glm::vec3 &minv, const glm::vec3 &maxv) const {
        glm::vec3 p;
        for (int i = 0; i < 6; i++) {
            p.x = planes[i].x >= 0.0f ? maxv.x : minv.x;
            p.y = planes[i].y >= 0.0f ? maxv.y : minv.y;
            p.z = planes[i].z >= 0.0f ? maxv.z : minv.z;
            float dist = planes[i].x * p.x + planes[i].y * p.y + planes[i].z * p.z + planes[i].w;
            if (dist < 0.0f) return false;
        }
        return true;
    }
};