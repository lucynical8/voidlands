#pragma once

#include <queue>
#include <mutex>
#include <thread>
#include <atomic>
#include <unordered_set>
#include <unordered_map>
#include <condition_variable>
#include <config.hpp>
#include <utilsx.hpp>
#include <camera.hpp>
#include <bmeshx.hpp>
#include <blocks.hpp>
#include <terras.hpp>
#include <lummis.hpp>
#include <greedy.hpp>

using glm::floor;
using glm::length;
using glm::radians;
using glm::translate;
using glm::perspective;

ivec3 worldtochunk(const vec3 &pos) {
    return floor(pos / 16.0f);
}

vec3 chunktoworld(const ivec3 &chunkpos) {
    return vec3{chunkpos} * 16.0f;
}

float chunkdist(const ivec3 &a, const ivec3 &b) {
    return length(vec3{a - b});
}

struct Chunkdraw {
    ivec3 pos;
    GLuint vao;
    int indxcnt;
};

struct Chunk {
    Bmesh bmesh;
    atomic<bool> ready;
    array<uint8, 4096> blocks;
};

struct Chunksys {
    atomic<bool> running = false;

    thread updatethr;
    unordered_map<ivec3, Chunk, ivec3hash> loadedmap;
    mutex meshesmtx;
    mutex blocksmtx;

    thread togenthr[5];
    unordered_set<ivec3, ivec3hash> togenset;
    condition_variable togencond;
    queue<ivec3> togenque;
    mutex togenmtx;

    thread tomeshthr[5];
    unordered_set<ivec3, ivec3hash> tomeshset;
    condition_variable tomeshcond;
    queue<ivec3> tomeshque;
    mutex tomeshmtx;

    thread toremthr;
    unordered_set<ivec3, ivec3hash> toremset;
    condition_variable toremcond;
    queue<ivec3> toremque;
    mutex toremmtx;

    vector<ivec3> gpumeshvec;
    vector<Bmesh> gpudelvec;

    atomic<int> rendist = 7;

    vec3 plrpos {0.0f};
    mutex plrposmtx;

    Terras terras;
    Lummis lummis;
    Greedy greedy;

    ~Chunksys() {
        stop();
    }

    void start() {
        running = true;
        updatethr = thread(&Chunksys::updateworker, this);
        for (int i = 0; i < 5; i++) {
            togenthr[i] = thread(&Chunksys::togenworker, this);
        }
        for (int i = 0; i < 5; i++) {
            tomeshthr[i] = thread(&Chunksys::tomeshworker, this);
        }
        toremthr = thread(&Chunksys::toremworker, this);
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
            lock_guard<mutex> lock1(meshesmtx);
            lock_guard<mutex> lock2(blocksmtx);
            for (auto &[chunkpos, chunk] : loadedmap) {
                chunk.bmesh.delself();
            }
            loadedmap.clear();
            for (auto &mesh : gpudelvec) {
                mesh.delself();
            }
            gpudelvec.clear();
        }
    }

    void updateworker() {
        while (running) {
            int rendist = this->rendist;
            ivec3 plrchunkpos {worldtochunk(getplrpos())};
            unordered_set<ivec3, ivec3hash> neededset;
            for (int x = -rendist; x <= rendist; x++) {
                for (int y = -rendist; y <= rendist; y++) {
                    for (int z = -rendist; z <= rendist; z++) {
                        ivec3 chunkpos {plrchunkpos + ivec3{x, y, z}};
                        if (chunkdist(plrchunkpos, chunkpos) > rendist) continue;
                        neededset.insert(chunkpos);
                    }
                }
            }
            {
                lock_guard<mutex> lock1(togenmtx);
                lock_guard<mutex> lock2(tomeshmtx);
                lock_guard<mutex> lock3(toremmtx);
                for (const auto &chunkpos : neededset) {
                    if (loadedmap.find(chunkpos) != loadedmap.end()) continue;
                    if (togenset.find(chunkpos) != togenset.end()) continue;
                    togenset.insert(chunkpos);
                    togenque.push(chunkpos);
                }
                for (const auto &[chunkpos, chunk] : loadedmap) {
                    if (neededset.find(chunkpos) == neededset.end()) {
                        if (chunkdist(plrchunkpos, chunkpos) <= rendist + 1) continue;
                        if (toremset.find(chunkpos) != toremset.end()) continue;
                        toremset.insert(chunkpos);
                        toremque.push(chunkpos);
                        continue;
                    }
                    if (!chunk.ready) {
                        if (tomeshset.find(chunkpos) == tomeshset.end()) {
                            for (int i = 0; i < 27; i++) {
                                ivec3 relpos {i/9%3-1, i/3%3-1, i/1%3-1};
                                if (loadedmap.find(chunkpos + relpos) == loadedmap.end()) goto skip;
                            }
                            tomeshset.insert(chunkpos);
                            tomeshque.push(chunkpos);
                        }
                    }
                    skip:;
                }
                togencond.notify_all();
                tomeshcond.notify_all();
                toremcond.notify_all();
            }
            this_thread::sleep_for(chrono::milliseconds(500));
        }
    }

    void togenworker() {
        while (running) {
            ivec3 chunkpos;
            {
                unique_lock<mutex> lock(togenmtx);
                togencond.wait(lock, [&] {return !togenque.empty() || !running;});
                if (!running) break;
                chunkpos = togenque.front();
                togenque.pop();
            }
            if (chunkdist(worldtochunk(getplrpos()), chunkpos) <= rendist) {
                genone(chunkpos);
            }
            {
                lock_guard<mutex> lock(togenmtx);
                togenset.erase(chunkpos);
            }
        }
    }

    void genone(const ivec3 &chunkpos) {
        array<uint8, 4096> blocks;
        terras.genblocks(blocks, chunkpos);
        {
            lock_guard<mutex> lock1(meshesmtx);
            lock_guard<mutex> lock2(blocksmtx);
            loadedmap[chunkpos].blocks = blocks;
            loadedmap[chunkpos].ready = false;
        }
    }

    void tomeshworker() {
        while (running) {
            ivec3 chunkpos;
            {
                unique_lock<mutex> lock(tomeshmtx);
                tomeshcond.wait(lock, [&] {return !tomeshque.empty() || !running;});
                if (!running) break;
                chunkpos = tomeshque.front();
                tomeshque.pop();
            }
            meshone(chunkpos, false);
            {
                lock_guard<mutex> lock(tomeshmtx);
                tomeshset.erase(chunkpos);
            }
        }
    }

    void meshone(const ivec3 &chunkpos, bool native) {
        Bmesh newmesh;
        array<uint8, 4096> blocks[27];
        {
            lock_guard<mutex> lock(blocksmtx);
            for (int i = 0; i < 27; i++) {
                ivec3 relpos {i/9%3-1, i/3%3-1, i/1%3-1};
                if (loadedmap.find(chunkpos + relpos) == loadedmap.end()) return;
                blocks[i] = loadedmap[chunkpos + relpos].blocks;
            }
        }
        array<uint8, 4096> lights[27];
        lummis.genlights(lights, blocks);
        if (native) greedy.genmeshnative(newmesh, blocks, lights);
        else greedy.genmeshgreedy(newmesh, blocks, lights);
        {
            lock_guard<mutex> lock(meshesmtx);
            if (loadedmap.find(chunkpos) != loadedmap.end()) {
                auto &chunk = loadedmap[chunkpos];
                chunk.bmesh.verts = move(newmesh.verts);
                chunk.bmesh.indxs = move(newmesh.indxs);
                gpumeshvec.push_back(chunkpos);
                chunk.ready = true;
            }
        }
    }

    void toremworker() {
        while (running) {
            ivec3 chunkpos;
            {
                unique_lock<mutex> lock(toremmtx);
                toremcond.wait(lock, [&] {return !toremque.empty() || !running;});
                if (!running) break;
                chunkpos = toremque.front();
                toremque.pop();
            }
            if (chunkdist(worldtochunk(getplrpos()), chunkpos) > rendist + 1) {
                remone(chunkpos);
            }
            {
                lock_guard<mutex> lock(toremmtx);
                toremset.erase(chunkpos);
            }
        }
    }

    void remone(const ivec3 &chunkpos) {
        lock_guard<mutex> lock1(meshesmtx);
        lock_guard<mutex> lock2(blocksmtx);
        auto &chunk = loadedmap[chunkpos];
        gpudelvec.push_back(chunk.bmesh);
        loadedmap.erase(chunkpos);
    }

    void gpumesh() {
        lock_guard<mutex> lock(meshesmtx);
        for (const auto &chunkpos : gpumeshvec) {
            if (loadedmap.find(chunkpos) == loadedmap.end()) continue;
            loadedmap[chunkpos].bmesh.crself();
        }
        for (auto &mesh : gpudelvec) {
            mesh.remself();
            mesh.delself();
        }
        gpumeshvec.clear();
        gpumeshvec.shrink_to_fit();
        gpudelvec.clear();
        gpudelvec.shrink_to_fit();
    }

    void drawself(const Camera &camera) {
        vector<Chunkdraw> todrawvec;
        {
            lock_guard<mutex> lock(meshesmtx);
            for (const auto &[chunkpos, chunk] : loadedmap) {
                if (!chunk.ready) continue;
                int indxcnt = chunk.bmesh.indxcnt;
                todrawvec.push_back({chunkpos, chunk.bmesh.vao, indxcnt});
            }
        }
        {
            bmeshshd.use();
            bmeshshd.setint("blocktexarr", 0);
            float whratio = (float)scrw / (float)scrh;
            bmeshshd.setmat4("view", camera.getviewmat());
            bmeshshd.setmat4("proj", perspective(radians(camera.fov), whratio, 0.1f, 500.0f));
        }
        {
            glLineWidth(2.0f);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D_ARRAY, blocktexarr);
            for (const auto &chunk : todrawvec) {
                vec3 minv {chunk.pos * 16};
                vec3 maxv {minv + vec3{16.0f}};
                if (!camera.frustum.isvisible(minv, maxv)) continue;
                mat4 model {translate(mat4{1.0f}, chunktoworld(chunk.pos))};
                bmeshshd.setmat4("model", model);
                glBindVertexArray(chunk.vao);
                glDrawElements(GL_TRIANGLES, chunk.indxcnt, GL_UNSIGNED_INT, 0);
            }
        }
    }

    void setblock(const ivec3 &pos, uint8 block) {
        ivec3 chunkpos {worldtochunk(pos)};
        ivec3 blockpos {pos - chunkpos * 16};
        {
            lock_guard<mutex> lock(blocksmtx);
            if (loadedmap.find(chunkpos) == loadedmap.end()) return;
            loadedmap[chunkpos].blocks[blockindx(blockpos)] = block;
        }
        {
            for (int i = 0; i < 27; i++) {
                ivec3 relpos {i/9%3-1, i/3%3-1, i/1%3-1};
                thread(&Chunksys::meshone, this, chunkpos + relpos, false).detach();
            }
        }
    }

    uint8 getblock(const ivec3 &pos) {
        int idx = 13;
        ivec3 chunkpos {worldtochunk(pos)};
        ivec3 blockpos {pos - chunkpos * 16};
        if (blockpos.x < 0) idx = 4;
        if (blockpos.x >= 16) idx = 22;
        if (blockpos.y < 0) idx = 10;
        if (blockpos.y >= 16) idx = 16;
        if (blockpos.z < 0) idx = 12;
        if (blockpos.z >= 16) idx = 14;
        {
            lock_guard<mutex> lock(blocksmtx);
            ivec3 relpos {idx/9%3-1, idx/3%3-1, idx/1%3-1};
            if (loadedmap.find(chunkpos + relpos) == loadedmap.end()) return 0;
            return loadedmap[chunkpos + relpos].blocks[blockindx(blockpos)];
        }
    }

    void setplrpos(const vec3 &pos) {
        lock_guard<mutex> lock(plrposmtx);
        plrpos = pos;
    }

    vec3 getplrpos() {
        lock_guard<mutex> lock(plrposmtx);
        return plrpos;
    }

    int blockindx(const ivec3 &pos) {
        return pos.x * 256 + pos.y * 16 + pos.z;
    }
};