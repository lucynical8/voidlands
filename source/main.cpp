#define STB_IMAGE_IMPLEMENTATION

#include <config.hpp>
#include <wrapps.hpp>
#include <textrx.hpp>
#include <crossx.hpp>
#include <player.hpp>
#include <sweptx.hpp>
#include <blocks.hpp>
#include <chunks.hpp>

float lframe = 0.0f;

void updatedt() {
    float cframe = gettime();
    dtime = cframe - lframe;
    lframe = cframe;
}

Textrx textrx;
Crossx crossx;
Player player;
Sweptx sweptx;
Blocksys blocksys;
Chunksys chunksys;

bool debug = false;
bool cursor = false;
bool collis = false;
bool gravity = false;

string onoff(bool b) {
    if (b) return "on";
    else return "off";
}

string maintext;

void updmaintext() {
    static int cfps = 0;
    static float last = 0.0f;
    float now = gettime();
    if (now - last > 0.5f) {
        cfps = (int)(1.0f / dtime);
        last = now;
    }
    string sfps = "fps: " + to_string(cfps);
    string srendist = "\nrender distance: " + to_string(chunksys.rendist);
    string instructs = (string)"\nk/l - +/- rend by 1" +
                       (string)"\nx - toggle debug - " + onoff(debug) +
                       (string)"\nc - toggle cursor - " + onoff(cursor) +
                       (string)"\nv - toggle collision - " + onoff(collis) +
                       (string)"\ng - toggle gravity - " + onoff(gravity) +
                       (string)"\n1 to 7 - change blocks";
    string sblock = "\nselected block: " + blocknames[player.selecblock - 1];
    maintext = sfps + srendist + instructs + sblock;
}

int main() {
    initglfw();
    crwindow(window, scrw, scrh, "voidlands");
    disabcur(window);
    initglad();
    enabcull();
    enabdepth();
    setfbscb(window);
    setkeycb(window);
    setcposcb(window);
    setmbtncb(window);
    crbmeshshd();
    crtextrxshd();
    crcrossxshd();
    textrx.crself();
    crossx.crself();
    blocksys.loadtexs();
    blocksys.crmeshes();
    glfwSwapInterval(1);
    player.camera.speed = 5.0f;
    chunksys.rendist = 7;
    chunksys.start();
    while (!shclose(window)) {
        updatedt();
        clearscr();
        pollevents();
        updmaintext();
        prinput(window);
        chunksys.setplrpos(player.camera.pos);
        chunksys.gpumesh();
        chunksys.drawself(player.camera);
        textrx.render(maintext, vec4{1.0f}, 3.0f, 20.0f, 20.0f);
        crossx.drawself();
        swapbuff(window);
    }
    chunksys.stop();
    textrx.delself();
    crossx.delself();
    exitglfw();
    return 0;
}

void prinput(GLFWwindow *window) {
    vector<Direc> dirs;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        dirs.push_back(Fw);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        dirs.push_back(Bk);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        dirs.push_back(Lt);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        dirs.push_back(Rt);
    }
    if (!gravity && glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        dirs.push_back(Up);
    }
    if (!gravity && glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
        dirs.push_back(Dn);
    }
    player.camera.prkeybr(dirs);
    bool jump = glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS;
    player.prmove(sweptx, chunksys, gravity, collis, jump, dtime);
    player.camera.updfrust();
}

void keycb(GLFWwindow *window, int key, int scn, int act, int mod) {
    if (key == GLFW_KEY_ESCAPE && act == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
    if (key == GLFW_KEY_X && act == GLFW_PRESS) {
        debug = !debug;
        if (debug) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        } else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
    }
    if (key == GLFW_KEY_C && act == GLFW_PRESS) {
        cursor = !cursor;
        if (cursor) enabcur(window);
        else disabcur(window);
    }
    if (key == GLFW_KEY_V && act == GLFW_PRESS) {
        player.onground = false;
        collis = !collis;
    }
    if (key == GLFW_KEY_G && act == GLFW_PRESS) {
        gravity = !gravity;
        player.vervel = 0.0f;
        player.onground = false;
        player.horvel = vec3{0.0f};
    }
    if (key == GLFW_KEY_K && act == GLFW_PRESS) {
        if (chunksys.rendist < 20) chunksys.rendist++;
    }
    if (key == GLFW_KEY_L && act == GLFW_PRESS) {
        if (chunksys.rendist > 1) chunksys.rendist--;
    }
    if (key == GLFW_KEY_1 && act == GLFW_PRESS) {
        player.selecblock = 1;
    }
    if (key == GLFW_KEY_2 && act == GLFW_PRESS) {
        player.selecblock = 2;
    }
    if (key == GLFW_KEY_3 && act == GLFW_PRESS) {
        player.selecblock = 3;
    }
    if (key == GLFW_KEY_4 && act == GLFW_PRESS) {
        player.selecblock = 4;
    }
    if (key == GLFW_KEY_5 && act == GLFW_PRESS) {
        player.selecblock = 5;
    }
    if (key == GLFW_KEY_6 && act == GLFW_PRESS) {
        player.selecblock = 6;
    }
    if (key == GLFW_KEY_7 && act == GLFW_PRESS) {
        player.selecblock = 7;
    }
}

void cposcb(GLFWwindow *window, double xpos, double ypos) {
    static float lastx = scrw / 2.0f;
    static float lasty = scrh / 2.0f;
    static bool fmouse = true;
    if (fmouse) {
        lastx = xpos;
        lasty = ypos;
        fmouse = false;
    }
    float xoff = xpos - lastx;
    float yoff = lasty - ypos;
    player.camera.prmouse(xoff, yoff);
    lastx = xpos;
    lasty = ypos;
}

void mbtncb(GLFWwindow *window, int btn, int act, int mod) {
    if (btn == GLFW_MOUSE_BUTTON_LEFT && act == GLFW_PRESS) {
        player.destroyblock(chunksys);
    }
    if (btn == GLFW_MOUSE_BUTTON_RIGHT && act == GLFW_PRESS) {
        player.placeblock(chunksys);
    }
}