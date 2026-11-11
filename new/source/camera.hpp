#pragma once

#include <glm/gtc/matrix_transform.hpp>
#include <config.hpp>
#include <frustx.hpp>

using glm::sin;
using glm::cos;
using glm::cross;
using glm::radians;
using glm::normalize;
using glm::perspective;

enum Direc {Fw, Bk, Lt, Rt, Up, Dn};
const vec3 worldup {0.0f, 1.0f, 0.0f};

struct Camera {
    vec3 pos {0.0f};
    vec3 vel {0.0f};

    vec3 front;
    vec3 right;
    vec3 up;

    float yaw = -90.0f;
    float pitch = 0.0f;
    float fov = 80.0f;

    float speed = 5.0f;
    float sens = 0.1f;

    Frustum frustum;

    Camera() {
        updvects();
        updfrust();
    }

    void updvects() {
        vec3 nfront;
        nfront.x = cos(radians(yaw)) * cos(radians(pitch));
        nfront.y = sin(radians(pitch));
        nfront.z = sin(radians(yaw)) * cos(radians(pitch));
        front = normalize(nfront);
        right = normalize(cross(front, worldup));
        up = normalize(cross(right, front));
    }

    void updfrust() {
        float whratio = (float)scrw / (float)scrh;
        mat4 proj {perspective(radians(fov), whratio, 0.1f, 500.0f)};
        frustum.update(proj * getviewmat());
    }

    void prkeybr(vector<Direc> dirs) {
        vec3 frontxz {normalize(vec3{front.x, 0.0f, front.z})};
        vec3 rightxz {normalize(vec3{right.x, 0.0f, right.z})};
        vec3 dpos {0.0f};
        vel = vec3{0.0f};
        for (Direc dir : dirs) {
            if (dir == Fw) dpos += frontxz;
            if (dir == Bk) dpos -= frontxz;
            if (dir == Lt) dpos -= rightxz;
            if (dir == Rt) dpos += rightxz;
            if (dir == Up) dpos += worldup;
            if (dir == Dn) dpos -= worldup;
        }
        if (dpos == vec3{0.0f}) return;
        vel = normalize(dpos) * speed * dtime;
    }

    void prmouse(float xoff, float yoff) {
        yaw += xoff * sens;
        pitch += yoff * sens;
        if (pitch > 89.0f) pitch = 89.0f;
        if (pitch < -89.0f) pitch = -89.0f;
        updvects();
        updfrust();
    }

    mat4 getviewmat() const {
        return lookAt(pos, pos + front, worldup);
    }
};