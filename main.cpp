#include <iostream>
#include <chrono>
#include <iomanip>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// OpenGL Extension Wrangler, must be included before GLFW
#include "GL/glew.h"

// GLFW, window handler
#include <GLFW/glfw3.h>

int main(void)
{
    GLFWwindow* window;

    if (!glfwInit()) {
        exit(EXIT_FAILURE);
    }

    // Core profile 3.3
    // See http://www.glfw.org/docs/latest/window.html#window_hints for more hints
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    // Open window with GLFW
    window = glfwCreateWindow(800, 600, "OpenGL template", NULL, NULL);
    if (!window)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    //Set the GLFW-context the current window
    glfwMakeContextCurrent(window);
    std::cout << glGetString(GL_VERSION) << "\n";

    /* Set up GLEW */
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
        /* Problem: glewInit failed, something is seriously wrong. */
        std::cout << "GLEW init error: " << glewGetErrorString(err) << std::endl;
        return -1;
    }

    // Init size
    int width, height;


    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        // Update window size
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);

        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);



        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();

        if (glfwGetKey(window, GLFW_KEY_ESCAPE)) {
            glfwSetWindowShouldClose(window, 1);
        }
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}