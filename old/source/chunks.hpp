#pragma once
#include <array>
#include <queue>
#include <mutex>
#include <thread>
#include <atomic>
#include <chrono>
#include <vector>
#include <iostream>
#include <algorithm>
#include <unordered_set>
#include <unordered_map>
#include <condition_variable>
#include <glm/glm.hpp>
#include <config.hpp>
#include <camera.hpp>
#include <bmeshx.hpp>
#include <blocks.hpp>
#include <terras.hpp>
#include <lummis.hpp>
#include <greedy.hpp>

glm::ivec3 worldtochunk(const glm::vec3 &pos) {
    return glm::floor(pos / 16.0f);
}

glm::vec3 chunktoworld(const glm::ivec3 &chunkpos) {
    return glm::vec3(chunkpos) * 16.0f;
}

float chunkdist(const glm::ivec3 &a, const glm::ivec3 &b) {
    return glm::length(glm::vec3(a - b));
}

struct Chunkdraw {
    glm::ivec3 pos;
    int indxscount;
    uint32_t vao;
};

struct Chunk {
    Bmesh mesh;
    std::atomic<bool> ready;
    std::array<uint8_t, 4096> blocks;
};

struct Chunksystem {
    std::atomic<bool> running;

    std::thread updatethr;
    std::unordered_map<glm::ivec3, Chunk, ivec3hash> loadedmap;
    std::mutex meshesmtx;
    std::mutex blocksmtx;

    std::thread togenthr[5];
    std::unordered_set<glm::ivec3, ivec3hash> togenset;
    std::condition_variable togencond;
    std::queue<glm::ivec3> togenque;
    std::mutex togenmtx;

    std::thread tomeshthr[5];
    std::unordered_set<glm::ivec3, ivec3hash> tomeshset;
    std::condition_variable tomeshcond;
    std::queue<glm::ivec3> tomeshque;
    std::mutex tomeshmtx;

    std::thread toremthr;
    std::unordered_set<glm::ivec3, ivec3hash> toremset;
    std::condition_variable toremcond;
    std::queue<glm::ivec3> toremque;
    std::mutex toremmtx;

    std::unordered_map<uint32_t, glm::ivec3> gpumeshmap;
    std::unordered_map<uint32_t, Bmesh> gpudelmap;

    std::atomic<int> rendist = 7;

    glm::vec3 playerpos = glm::vec3(0.0f);
    std::mutex playerposmtx;

    Terras terras;
    Lummis lummis;
    Greedy greedy;

    ~Chunksystem() {
        stop();
    }

    void start() {
        running = true;
        updatethr = std::thread(&Chunksystem::updateworker, this);
        for (int i = 0; i < 5; i++) {
            togenthr[i] = std::thread(&Chunksystem::togenworker, this);
        }
        for (int i = 0; i < 5; i++) {
            tomeshthr[i] = std::thread(&Chunksystem::tomeshworker, this);
        }
        toremthr = std::thread(&Chunksystem::toremworker, this);
    }

    void stop() {
        running = false;
        togencond.notify_all();
        tomeshcond.notify_all();
        toremcond.notify_all();
        if (updatethr.joinable()) updatethr.join();
        for (int i = 0; i < 5; i++) {
            if (togenthr[i].joinable()) togenthr[i].join();
        }
        for (int i = 0; i < 5; i++) {
            if (tomeshthr[i].joinable()) tomeshthr[i].join();
        }
        if (toremthr.joinable()) toremthr.join();
        {
            std::lock_guard<std::mutex> lock1(meshesmtx);
            std::lock_guard<std::mutex> lock2(blocksmtx);
            for (auto &[chunkpos, chunk] : loadedmap) {
                chunk.mesh.deleteself();
            }
            loadedmap.clear();
        }
    }

    void updateworker() {
        while (running) {
            int rendist = this->rendist;
            glm::ivec3 playerchunkpos = worldtochunk(getplayerpos());
            std::unordered_set<glm::ivec3, ivec3hash> neededset;
            for (int x = -rendist; x <= rendist; x++) {
                for (int y = -rendist; y <= rendist; y++) {
                    for (int z = -rendist; z <= rendist; z++) {
                        glm::ivec3 chunkpos = playerchunkpos + glm::ivec3(x, y, z);
                        if (chunkdist(playerchunkpos, chunkpos) > rendist) continue;
                        neededset.insert(chunkpos);
                    }
                }
            }
            {
                std::lock_guard<std::mutex> lock1(togenmtx);
                std::lock_guard<std::mutex> lock2(tomeshmtx);
                std::lock_guard<std::mutex> lock3(toremmtx);
                for (const auto &chunkpos : neededset) {
                    if (loadedmap.find(chunkpos) != loadedmap.end()) continue;
                    if (togenset.find(chunkpos) != togenset.end()) continue;
                    togenset.insert(chunkpos);
                    togenque.push(chunkpos);
                }
                for (const auto &[chunkpos, chunk] : loadedmap) {
                    if (neededset.find(chunkpos) == neededset.end()) {
                        if (chunkdist(playerchunkpos, chunkpos) <= rendist + 1) continue;
                        if (toremset.find(chunkpos) != toremset.end()) continue;
                        toremset.insert(chunkpos);
                        toremque.push(chunkpos);
                        continue;
                    }
                    if (chunk.ready) continue;
                    for (int i = 0; i < 27; i++) {
                        glm::ivec3 relpos = glm::ivec3(i/9%3-1, i/3%3-1, i/1%3-1);
                        if (loadedmap.find(chunkpos + relpos) == loadedmap.end()) goto skip;
                    }
                    if (tomeshset.find(chunkpos) != tomeshset.end()) continue;
                    tomeshset.insert(chunkpos);
                    tomeshque.push(chunkpos);
                    skip:;
                }
                togencond.notify_all();
                tomeshcond.notify_all();
                toremcond.notify_all();
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
    }

    void togenworker() {
        while (running) {
            glm::ivec3 chunkpos;
            {
                std::unique_lock<std::mutex> lock(togenmtx);
                togencond.wait(lock, [&] {return !togenque.empty() || !running;});
                if (!running) break;
                chunkpos = togenque.front();
                togenque.pop();
            }
            genone(chunkpos);
            {
                std::lock_guard<std::mutex> lock(togenmtx);
                togenset.erase(chunkpos);
            }
        }
    }

    void genone(const glm::ivec3 &chunkpos) {
        std::array<uint8_t, 4096> blocks;
        terras.genblocks(blocks, chunkpos);
        {
            std::lock_guard<std::mutex> lock1(meshesmtx);
            std::lock_guard<std::mutex> lock2(blocksmtx);
            loadedmap[chunkpos].blocks = blocks;
            loadedmap[chunkpos].ready = false;
        }
    }

    void tomeshworker() {
        while (running) {
            glm::ivec3 chunkpos;
            {
                std::unique_lock<std::mutex> lock(tomeshmtx);
                tomeshcond.wait(lock, [&] {return !tomeshque.empty() || !running;});
                if (!running) break;
                chunkpos = tomeshque.front();
                tomeshque.pop();
            }
            meshone(chunkpos, false);
            {
                std::lock_guard<std::mutex> lock(tomeshmtx);
                tomeshset.erase(chunkpos);
            }
        }
    }

    void meshone(const glm::ivec3 &chunkpos, bool native) {
        Bmesh newmesh;
        std::array<uint8_t, 4096> blocks[27];
        {
            std::lock_guard<std::mutex> lock(blocksmtx);
            for (int i = 0; i < 27; i++) {
                glm::ivec3 relpos = glm::ivec3(i/9%3-1, i/3%3-1, i/1%3-1);
                if (loadedmap.find(chunkpos + relpos) == loadedmap.end()) return;
                blocks[i] = loadedmap[chunkpos + relpos].blocks;
            }
        }
        std::array<uint8_t, 4096> lights[27];
        lummis.genlights(lights, blocks);
        if (native) greedy.genmeshnative(newmesh, blocks, lights);
        else greedy.genmeshgreedy(newmesh, blocks, lights);
        {
            std::lock_guard<std::mutex> lock(meshesmtx);
            auto &chunk = loadedmap[chunkpos];
            uint32_t vao = chunk.mesh.vao;
            uint32_t vbo = chunk.mesh.vbo;
            uint32_t ebo = chunk.mesh.ebo;
            if (vao != 0) {
                gpumeshmap[vao] = chunkpos;
                gpudelmap[vao] = Bmesh{vao, vbo, ebo};
            } else {
                gpumeshmap[(uint32_t)rand()] = chunkpos;
            }
            chunk.mesh.verts = std::move(newmesh.verts);
            chunk.mesh.indxs = std::move(newmesh.indxs);
            chunk.ready = true;
        }
    }

    void toremworker() {
        while (running) {
            glm::ivec3 chunkpos;
            {
                std::unique_lock<std::mutex> lock(toremmtx);
                toremcond.wait(lock, [&] {return !toremque.empty() || !running;});
                if (!running) break;
                chunkpos = toremque.front();
                toremque.pop();
            }
            remone(chunkpos);
            {
                std::lock_guard<std::mutex> lock(toremmtx);
                toremset.erase(chunkpos);
            }
        }
    }

    void remone(const glm::ivec3 &chunkpos) {
        std::lock_guard<std::mutex> lock1(meshesmtx);
        std::lock_guard<std::mutex> lock2(blocksmtx);
        const auto &chunk = loadedmap[chunkpos];
        uint32_t vao = chunk.mesh.vao;
        uint32_t vbo = chunk.mesh.vbo;
        uint32_t ebo = chunk.mesh.ebo;
        gpudelmap[vao] = Bmesh{vao, vbo, ebo};
        loadedmap.erase(chunkpos);
    }

    void gpumesh() {
        std::lock_guard<std::mutex> lock(meshesmtx);
        for (const auto &[vao, chunkpos] : gpumeshmap) {
            if (loadedmap.find(chunkpos) == loadedmap.end()) continue;
            loadedmap[chunkpos].mesh.createself();
        }
        std::vector<uint32_t> todelvec;
        for (auto &[vao, mesh] : gpudelmap) {
            if (gpumeshmap.find(vao) != gpumeshmap.end()) continue;
            todelvec.push_back(vao);
            mesh.deleteself();
        }
        for (auto vao : todelvec) {
            gpudelmap.erase(vao);
        }
        gpumeshmap.clear();
    }

    void drawself(const Camera &camera) {
        std::vector<Chunkdraw> todrawvec;
        {
            std::lock_guard<std::mutex> lock(meshesmtx);
            for (const auto &[chunkpos, chunk] : loadedmap) {
                if (!chunk.ready) continue;
                int indxscount = chunk.mesh.indxscount;
                todrawvec.push_back({chunkpos, indxscount, chunk.mesh.vao});
            }
        }
        {
            glLineWidth(2.0f);
            bmeshshader.use();
            bmeshshader.setint("blocktexarray", 0);
            bmeshshader.setmat4("view", camera.getviewmatrix());
            float whratio = (float)screenwidth / (float)screenheight;
            bmeshshader.setmat4("proj", glm::perspective(glm::radians(camera.fov), whratio, 0.1f, 500.0f));
        }
        {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D_ARRAY, blocktexarray);
            for (const auto &chunk : todrawvec) {
                glm::vec3 minv = glm::vec3(chunk.pos) * 16.0f;
                glm::vec3 maxv = minv + glm::vec3(16.0f);
                if (!camera.frustum.isvisible(minv, maxv)) continue;
                glm::mat4 model = glm::translate(glm::mat4(1.0f), chunktoworld(chunk.pos));
                bmeshshader.setmat4("model", model);
                glBindVertexArray(chunk.vao);
                glDrawElements(GL_TRIANGLES, chunk.indxscount, GL_UNSIGNED_INT, 0);
            }
        }
    }

    void setblock(const glm::ivec3 &pos, uint8_t block) {
        glm::ivec3 chunkpos = worldtochunk(glm::vec3(pos));
        glm::ivec3 blockpos = pos - chunkpos * 16;
        {
            std::lock_guard<std::mutex> lock(blocksmtx);
            if (loadedmap.find(chunkpos) == loadedmap.end()) return;
            loadedmap[chunkpos].blocks[blockindx(blockpos)] = block;
        }
        {
            for (int i = 0; i < 27; i++) {
                glm::ivec3 relpos = glm::ivec3(i/9%3-1, i/3%3-1, i/1%3-1);
                std::thread(&Chunksystem::meshone, this, chunkpos + relpos, false).detach();
            }
        }
    }

    uint8_t getblock(const glm::ivec3 &pos) {
        int idx = 13;
        glm::ivec3 chunkpos = worldtochunk(glm::vec3(pos));
        glm::ivec3 blockpos = pos - chunkpos * 16;
        if (blockpos.x < 0) idx = 4;
        if (blockpos.x >= 16) idx = 22;
        if (blockpos.y < 0) idx = 10;
        if (blockpos.y >= 16) idx = 16;
        if (blockpos.z < 0) idx = 12;
        if (blockpos.z >= 16) idx = 14;
        {
            std::lock_guard<std::mutex> lock(blocksmtx);
            glm::ivec3 relpos = glm::ivec3(idx/9%3-1, idx/3%3-1, idx/1%3-1);
            if (loadedmap.find(chunkpos + relpos) == loadedmap.end()) return 0;
            return loadedmap[chunkpos + relpos].blocks[blockindx(blockpos)];
        }
    }

    void setplayerpos(const glm::vec3 &pos) {
        std::lock_guard<std::mutex> lock(playerposmtx);
        playerpos = pos;
    }

    glm::vec3 getplayerpos() {
        std::lock_guard<std::mutex> lock(playerposmtx);
        return playerpos;
    }

    int blockindx(const glm::ivec3 &pos) {
        return pos.x * 256 + pos.y * 16 + pos.z;
    }
};