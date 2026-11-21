#pragma once

#include <config.hpp>
#include <camera.hpp>
#include <rayxer.hpp>
#include <chunks.hpp>
#include <sweptx.hpp>

struct Player {
    Camera camera;
    Rayxer rayxer;

    uint8 selecblock = 1;

    float vervel = 0.0f;
    float graval = 30.0f;
    float jmpvel = 8.8f;
    float horacc = 30.0f;
    float hordec = 25.0f;
    float airacc = 12.0f;
    float airdec = 2.0f;
    bool onground = false;
    vec3 horvel {0.0f};

    vec3 hitboxmin {-0.3f, -1.6f, -0.3f};
    vec3 hitboxmax {0.3f, 0.2f, 0.3f};

    bool checksupport(Sweptx &sweptx, Chunksys &chunksys) {
        vec3 pos {camera.pos};
        vec3 vel {0.0f, -0.01f, 0.0f};
        return sweptx.resolblocks(pos, vel, hitboxmin, hitboxmax, chunksys).floor;
    }

    void prmove(Sweptx &sweptx, Chunksys &chunksys, bool gravity, bool collis, bool jump, float dt) {
        if (gravity) {
            onground = collis && vervel <= 0.0f && checksupport(sweptx, chunksys);
            if (onground) vervel = 0.0f;
            vec3 target {0.0f};
            if (dt > 0.0f) {
                target = vec3{camera.vel.x, 0.0f, camera.vel.z} / dt;
            }
            vec3 change {target - horvel}; float rate;
            if (target == vec3{0.0f}) {
                if (onground) rate = hordec;
                else rate = airdec;
            } else {
                if (onground) rate = horacc;
                else rate = airacc;
            }
            float amount = length(change);
            if (amount > rate * dt) {
                change *= rate * dt / amount;
            }
            horvel += change;
            camera.vel.x = horvel.x * dt;
            camera.vel.z = horvel.z * dt;
            if (jump && onground) {
                vervel = jmpvel;
                onground = false;
            }
            vervel -= graval * dt;
            camera.vel.y = vervel * dt;
        }
        if (collis) {
            Colres colres {sweptx.resolblocks(camera.pos, camera.vel, hitboxmin, hitboxmax, chunksys)};
            if (gravity) {
                if (colres.x) horvel.x = 0.0f;
                if (colres.z) horvel.z = 0.0f;
                if ((colres.floor && vervel < 0.0f) || (colres.ceil && vervel > 0.0f)) vervel = 0.0f;
                onground = vervel <= 0.0f && checksupport(sweptx, chunksys);
            }
        } else {
            camera.pos += camera.vel;
            onground = false;
        }
        if (!gravity) {
            horvel = vec3{0.0f};
        }
    }

    void destroyblock(Chunksys &chunksys) {
        Rayhit rayhit;
        rayxer.raycast(rayhit, camera.pos, camera.front, chunksys);
        if (!rayhit.hit) return;
        chunksys.setblock(rayhit.pos, 0);
    }

    void placeblock(Chunksys &chunksys) {
        Rayhit rayhit;
        rayxer.raycast(rayhit, camera.pos, camera.front, chunksys);
        if (!rayhit.hit) return;
        ivec3 placepos {rayhit.pos + rayhit.nor};
        if (chunksys.getblock(placepos) != 0) return;
        vec3 playermin {camera.pos + hitboxmin};
        vec3 playermax {camera.pos + hitboxmax};
        vec3 blockmin {placepos};
        vec3 blockmax {blockmin + vec3{1.0f}};
        bool intersects = (playermin.x < blockmax.x && playermax.x > blockmin.x) &&
                          (playermin.y < blockmax.y && playermax.y > blockmin.y) &&
                          (playermin.z < blockmax.z && playermax.z > blockmin.z);
        if (intersects) return;
        chunksys.setblock(placepos, selecblock);
    }
};