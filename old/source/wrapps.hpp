#pragma once
#include <string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <config.hpp>

void initglfw() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

void createwindow(GLFWwindow *&window, int width, int height, const std::string &title) {
    window = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(0);
}

void fbscb(GLFWwindow *window, int width, int height) {
    screenwidth = width;
    screenheight = height;
    glViewport(0, 0, width, height);
}

void setfbscb(GLFWwindow *window) {
    glfwSetFramebufferSizeCallback(window, fbscb);
}

void processinput(GLFWwindow *window);

void keycb(GLFWwindow *window, int key, int scancode, int action, int mods);

void cposcb(GLFWwindow *window, double xpos, double ypos);

void mbtncb(GLFWwindow *window, int button, int action, int mods);

void setkeycb(GLFWwindow *window) {
    glfwSetKeyCallback(window, keycb);
}

void setcposcb(GLFWwindow *window) {
    glfwSetCursorPosCallback(window, cposcb);
}

void setmbtncb(GLFWwindow *window) {
    glfwSetMouseButtonCallback(window, mbtncb);
}

void enablecursor(GLFWwindow *window) {
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

void disablecursor(GLFWwindow *window) {
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void initglad() {
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
}

void enabledepth() {
    glEnable(GL_DEPTH_TEST);
}

void enablecull() {
    glEnable(GL_CULL_FACE);
}

void exitglfw() {
    glfwTerminate();
}

void pollevents() {
    glfwPollEvents();
}

void clearscreen() {
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void swapbuffers(GLFWwindow *window) {
    glfwSwapBuffers(window);
}

bool shouldclose(GLFWwindow *window) {
    return glfwWindowShouldClose(window);
}

float gettime() {
    return glfwGetTime();
}