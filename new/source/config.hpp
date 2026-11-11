#pragma once

#include <array>
#include <vector>
#include <string>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <algorithm>
#include <glm/glm.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
using namespace std;

using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::mat4;
using glm::ivec3;

using uint8 = uint8_t;
using uint32 = uint32_t;

GLFWwindow *window;
float dtime = 0.0f;
int scrw = 1200;
int scrh = 800;