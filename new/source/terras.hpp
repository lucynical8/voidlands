#pragma once

#include <ctime>
#include <fast/noisel.h>
#include <config.hpp>

struct Terras {
    FastNoiseLite cavenoise;
    FastNoiseLite tuffnoise;
    FastNoiseLite lummnoise;
    FastNoiseLite tempnoise;

    Terras() {
        initnoise();
    }

    void genblocks(array<uint8, 4096> &blocks, const ivec3 &chunkpos) {
        fill(blocks.begin(), blocks.end(), 0);
        for (int x = 0; x < 16; x++) {
            for (int y = 0; y < 16; y++) {
                for (int z = 0; z < 16; z++) {
                    float nx = chunkpos.x * 16 + x;
                    float ny = chunkpos.y * 16 + y;
                    float nz = chunkpos.z * 16 + z;
                    float caveval = cavenoise.GetNoise(nx, ny, nz);
                    float tuffval = tuffnoise.GetNoise(nx, ny, nz);
                    float lummval = lummnoise.GetNoise(nx, ny, nz);
                    float tempval = tempnoise.GetNoise(nx, ny, nz);
                    int indx = blockindx({x, y, z});
                    if (tempval > 0.4f) {
                        if (caveval < 0.0f) blocks[indx] = 5;
                    } else if (tempval < -0.4f) {
                        if (caveval < 0.1f) blocks[indx] = 4;
                    } else {
                        if (caveval < 0.2f) {
                            if (tuffval > 0.5f) blocks[indx] = 2;
                            else blocks[indx] = 1;
                        }
                    }
                    if (blocks[indx] == 0) continue;
                    if (lummval > 0.7f) blocks[indx] = 3;
                }
            }
        }
    }

    void initnoise() {
        srand(time(nullptr));
        cavenoise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
        tuffnoise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
        lummnoise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
        cavenoise.SetFractalType(FastNoiseLite::FractalType_FBm);
        tuffnoise.SetFractalType(FastNoiseLite::FractalType_FBm);
        lummnoise.SetFractalType(FastNoiseLite::FractalType_FBm);
        cavenoise.SetFractalLacunarity(0.3f);
        tuffnoise.SetFractalLacunarity(0.3f);
        lummnoise.SetFractalLacunarity(0.3f);
        cavenoise.SetFractalGain(0.2f);
        tuffnoise.SetFractalGain(0.2f);
        lummnoise.SetFractalGain(0.2f);
        cavenoise.SetFrequency(0.1f);
        tuffnoise.SetFrequency(0.1f);
        lummnoise.SetFrequency(0.1f);
        cavenoise.SetFractalOctaves(3);
        tuffnoise.SetFractalOctaves(3);
        lummnoise.SetFractalOctaves(3);
        cavenoise.SetSeed(rand());
        tuffnoise.SetSeed(rand());
        lummnoise.SetSeed(rand());
        tempnoise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
        tempnoise.SetFractalType(FastNoiseLite::FractalType_FBm);
        tempnoise.SetFractalLacunarity(0.4f);
        tempnoise.SetFractalGain(0.4f);
        tempnoise.SetFrequency(0.007f);
        tempnoise.SetFractalOctaves(4);
        tempnoise.SetSeed(rand());
    }

    int blockindx(const ivec3 &pos) {
        return pos.x * 256 + pos.y * 16 + pos.z;
    }
};