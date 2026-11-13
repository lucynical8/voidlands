#pragma once

#include <config.hpp>

void initglad() {
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
}

void initglfw() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

void crwindow(GLFWwindow *&window, int width, int height, const string &title) {
    window = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(0);
}

void fbscb(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
    scrw = width; scrh = height;
}

void setfbscb(GLFWwindow *window) {
    glfwSetFramebufferSizeCallback(window, fbscb);
}

void keycb(GLFWwindow *window, int key, int scn, int act, int mod);

void setkeycb(GLFWwindow *window) {
    glfwSetKeyCallback(window, keycb);
}

void mbtncb(GLFWwindow *window, int btn, int act, int mod);

void setmbtncb(GLFWwindow *window) {
    glfwSetMouseButtonCallback(window, mbtncb);
}

void cposcb(GLFWwindow *window, double xpos, double ypos);

void setcposcb(GLFWwindow *window) {
    glfwSetCursorPosCallback(window, cposcb);
}

void clearscr() {
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void enabcur(GLFWwindow *window) {
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

void disabcur(GLFWwindow *window) {
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

bool shclose(GLFWwindow *window) {
    return glfwWindowShouldClose(window);
}

void swapbuff(GLFWwindow *window) {
    glfwSwapBuffers(window);
}

void enabdepth() {
    glEnable(GL_DEPTH_TEST);
}

void enabcull() {
    glEnable(GL_CULL_FACE);
}

void pollevents() {
    glfwPollEvents();
}

void exitglfw() {
    glfwTerminate();
}

float gettime() {
    return glfwGetTime();
}

void prinput(GLFWwindow *window);