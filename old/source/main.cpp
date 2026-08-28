#include <string>
#include <vector>
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <wrapps.hpp>
#include <config.hpp>
#include <textrx.hpp>
#include <crossx.hpp>
#include <player.hpp>
#include <sweptx.hpp>
#include <bmeshx.hpp>
#include <blocks.hpp>
#include <chunks.hpp>

int screenwidth = 1200;
int screenheight = 800;
GLFWwindow *window;

float deltatime = 0.0f;
float lastframe = 0.0f;

void updatedt() {
    float currframe = gettime();
    deltatime = currframe - lastframe;
    lastframe = currframe;
}

Textrx textrx;
Crossx crossx;
Player player;
Sweptx sweptx;
Blocksystem blocksystem;
Chunksystem chunksystem;

bool debug = false;
bool cursor = false;
bool collision = false;

std::string maintext;

std::string onoff(bool b) {
    if (b) return "on";
    else return "off";
}

void updatemaintext() {
    static int cfps = 0;
    float now = gettime();
    static float last = 0.0f;
    if (now - last > 1.0f) {
        cfps = (int)(1.0f / deltatime);
        last = now;
    }
    std::string sfps = "fps: " + std::to_string(cfps);
    std::string srendist = "\nrender distance: " + std::to_string(chunksystem.rendist);
    std::string instructions = (std::string)"\ni/o - +/- fps by 10" +
                               (std::string)"\nk/l - +/- rend by 1" +
                               (std::string)"\nx - toggle debug - " + onoff(debug) +
                               (std::string)"\nc - toggle cursor - " + onoff(cursor) +
                               (std::string)"\nv - toggle collision - " + onoff(collision) +
                               (std::string)"\n1 to 7 - change blocks";
    std::string sblock = "\nselected block: " + blocknames[player.selecblock - 1];
    maintext = sfps + srendist + instructions + sblock;
}

float fps = 150.0f;

int main() {
    initglfw();
    createwindow(window, screenwidth, screenheight, "voidlands");
    disablecursor(window);
    initglad();
    enabledepth();
    enablecull();
    setfbscb(window);
    setkeycb(window);
    setcposcb(window);
    setmbtncb(window);
    createcrossxshader();
    textrx.createself();
    createbmeshshader();
    crossx.createself();
    createtextrxshader();
    blocksystem.loadtextures();
    blocksystem.createmeshes();
    player.camera.speed = 5.0f;
    chunksystem.rendist = 7;
    chunksystem.start();
    while (!shouldclose(window)) {
        updatedt();
        pollevents();
        updatemaintext();
        processinput(window);
        chunksystem.setplayerpos(player.camera.pos);
        chunksystem.gpumesh();
        clearscreen();
        chunksystem.drawself(player.camera);
        textrx.render(maintext, glm::vec4(1.0f), 3.0f, 20.0f, 20.0f);
        crossx.drawself();
        swapbuffers(window);
        float elapsed = gettime() - lastframe;
        float sleeptime = 1.0f / fps - elapsed;
        if (sleeptime > 0.0f) {
            std::this_thread::sleep_for(std::chrono::duration<float>(sleeptime));
        }
    }
    chunksystem.stop();
    textrx.deleteself();
    crossx.deleteself();
    exitglfw();
    return 0;
}

void processinput(GLFWwindow *window) {
    std::vector<Direction> dirs;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        dirs.push_back(Forward);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        dirs.push_back(Backward);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        dirs.push_back(Left);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        dirs.push_back(Right);
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        dirs.push_back(Up);
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
        dirs.push_back(Down);
    }
    player.camera.processkeyboard(dirs);
    if (collision) {
        sweptx.resolveblocks(player.camera.pos, player.camera.vel, player.hitboxmin, player.hitboxmax, chunksystem);
    } else {
        player.camera.pos += player.camera.vel;
    }
    player.camera.updatefrustum();
}

void keycb(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
    if (key == GLFW_KEY_X && action == GLFW_PRESS) {
        debug = !debug;
        if (debug) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        } else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
    }
    if (key == GLFW_KEY_C && action == GLFW_PRESS) {
        cursor = !cursor;
        if (cursor) enablecursor(window);
        else disablecursor(window);
    }
    if (key == GLFW_KEY_V && action == GLFW_PRESS) {
        collision = !collision;
    }
    if (key == GLFW_KEY_I && action == GLFW_PRESS) {
        fps += 10.0f;
    }
    if (key == GLFW_KEY_O && action == GLFW_PRESS) {
        if (fps - 10.0f >= 10.0f) fps -= 10.0f;
    }
    if (key == GLFW_KEY_K && action == GLFW_PRESS) {
        chunksystem.rendist++;
    }
    if (key == GLFW_KEY_L && action == GLFW_PRESS) {
        if (chunksystem.rendist > 1) chunksystem.rendist--;
    }
    if (key == GLFW_KEY_1 && action == GLFW_PRESS) {
        player.selecblock = 1;
    }
    if (key == GLFW_KEY_2 && action == GLFW_PRESS) {
        player.selecblock = 2;
    }
    if (key == GLFW_KEY_3 && action == GLFW_PRESS) {
        player.selecblock = 3;
    }
    if (key == GLFW_KEY_4 && action == GLFW_PRESS) {
        player.selecblock = 4;
    }
    if (key == GLFW_KEY_5 && action == GLFW_PRESS) {
        player.selecblock = 5;
    }
    if (key == GLFW_KEY_6 && action == GLFW_PRESS) {
        player.selecblock = 6;
    }
    if (key == GLFW_KEY_7 && action == GLFW_PRESS) {
        player.selecblock = 7;
    }
}

void cposcb(GLFWwindow *window, double xpos, double ypos) {
    static float lastx = screenwidth / 2.0f;
    static float lasty = screenheight / 2.0f;
    static bool firstmouse = true;
    if (firstmouse) {
        lastx = xpos;
        lasty = ypos;
        firstmouse = false;
    }
    float xoffset = xpos - lastx;
    float yoffset = lasty - ypos;
    player.camera.processmouse(xoffset, yoffset);
    lastx = xpos;
    lasty = ypos;
}

void mbtncb(GLFWwindow *window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        player.destroyblock(chunksystem);
    }
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
        player.placeblock(chunksystem);
    }
}