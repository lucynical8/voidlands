#pragma once

#include <config.hpp>
#include <camera.hpp>
#include <rayxer.hpp>
#include <chunks.hpp>

struct Player {
    Camera camera;
    Rayxer rayxer;

    uint8 selecblock = 1;

    vec3 hitboxmin {-0.3f, -1.6f, -0.3f};
    vec3 hitboxmax {0.3f, 0.2f, 0.3f};

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