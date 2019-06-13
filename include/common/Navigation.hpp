#pragma once

#include <GLFW/glfw3.h>
#include <cmath>

#ifndef M_PI
#define M_PI (3.141592653589793)
#endif

class KeyTranslator {

public:
    float horizontal;
    float vertical; // TODO
    float zoom;

private:
    double lastTime;

public:
    void init(GLFWwindow *window);
    void poll(GLFWwindow *window);
};

class MouseRotator {

public:
    float yaw;
    float pitch;
    float roll; // TODO

private:
    double lastX;
    double lastY;
    int lastLeft;
    int lastRight; // TODO
    const float SENSITIVITY = 0.1f;

public:
    void init(GLFWwindow *window);
    void poll(GLFWwindow *window);
};