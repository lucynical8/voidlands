#pragma once
#include <glm/glm.hpp>
#include <camera.hpp>
#include <rayxer.hpp>
#include <chunks.hpp>

struct Player {
    Camera camera;
    Rayxer rayxer;

    uint8_t selecblock = 1;

    glm::vec3 hitboxmin = -glm::vec3(0.3f, 1.6f, 0.3f);
    glm::vec3 hitboxmax = glm::vec3(0.3f, 0.2f, 0.3f);

    void destroyblock(Chunksystem &chunksystem) {
        Rayhit rayhit;
        rayxer.raycast(rayhit, camera.pos, camera.front, chunksystem);
        if (!rayhit.hit) return;
        chunksystem.setblock(rayhit.pos, 0);
    }

    void placeblock(Chunksystem &chunksystem) {
        Rayhit rayhit;
        rayxer.raycast(rayhit, camera.pos, camera.front, chunksystem);
        if (!rayhit.hit) return;
        glm::ivec3 placepos = rayhit.pos + glm::ivec3(rayhit.nor);
        if (chunksystem.getblock(placepos) != 0) return;
        glm::vec3 playermin = camera.pos + hitboxmin;
        glm::vec3 playermax = camera.pos + hitboxmax;
        glm::vec3 blockmin = glm::vec3(placepos);
        glm::vec3 blockmax = blockmin + glm::vec3(1.0f);
        bool intersects = (playermin.x < blockmax.x && playermax.x > blockmin.x) &&
                          (playermin.y < blockmax.y && playermax.y > blockmin.y) &&
                          (playermin.z < blockmax.z && playermax.z > blockmin.z);
        if (intersects) return;
        chunksystem.setblock(placepos, selecblock);
    }
};