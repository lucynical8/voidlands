#pragma once
#include <vector>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <config.hpp>
#include <frustx.hpp>

enum Direction {Forward, Backward, Left, Right, Up, Down};

const glm::vec3 worldup = glm::vec3(0.0f, 1.0f, 0.0f);

struct Camera {
    glm::vec3 pos = glm::vec3(0.0f);
    glm::vec3 vel = glm::vec3(0.0f);

    glm::vec3 front;
    glm::vec3 right;
    glm::vec3 up;

    float yaw = -90.0f;
    float pitch = 0.0f;
    float fov = 80.0f;

    float speed = 5.0f;
    float sens = 0.1f;

    Frustum frustum;

    Camera() {
        updatevectors();
        updatefrustum();
    }

    void updatevectors() {
        glm::vec3 newfront;
        newfront.x = glm::cos(glm::radians(yaw)) * glm::cos(glm::radians(pitch));
        newfront.y = glm::sin(glm::radians(pitch));
        newfront.z = glm::sin(glm::radians(yaw)) * glm::cos(glm::radians(pitch));
        front = glm::normalize(newfront);
        right = glm::normalize(glm::cross(front, worldup));
        up = glm::normalize(glm::cross(right, front));
    }

    void updatefrustum() {
        glm::mat4 view = getviewmatrix();
        float whratio = (float)screenwidth / (float)screenheight;
        glm::mat4 proj = glm::perspective(glm::radians(fov), whratio, 0.1f, 500.0f);
        frustum.update(proj * view);
    }

    void processkeyboard(std::vector<Direction> dirs) {
        glm::vec3 frontxz = glm::normalize(glm::vec3(front.x, 0.0f, front.z));
        glm::vec3 rightxz = glm::normalize(glm::vec3(right.x, 0.0f, right.z));
        glm::vec3 deltapos = glm::vec3(0.0f);
        vel = glm::vec3(0.0f);
        for (Direction dir : dirs) {
            if (dir == Forward) deltapos += frontxz;
            if (dir == Backward) deltapos -= frontxz;
            if (dir == Left) deltapos -= rightxz;
            if (dir == Right) deltapos += rightxz;
            if (dir == Up) deltapos += worldup;
            if (dir == Down) deltapos -= worldup;
        }
        if (deltapos == glm::vec3(0.0f)) return;
        vel = glm::normalize(deltapos) * speed * deltatime;
    }

    void processmouse(float xoffset, float yoffset) {
        yaw += xoffset * sens;
        pitch += yoffset * sens;
        if (pitch > 89.0f) pitch = 89.0f;
        if (pitch < -89.0f) pitch = -89.0f;
        updatevectors();
        updatefrustum();
    }

    glm::mat4 getviewmatrix() const {
        return glm::lookAt(pos, pos + front, worldup);
    }
};