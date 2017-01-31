#include <iostream>
#include <chrono>
#include <iomanip>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// OpenGL Extension Wrangler, must be included before GLFW
#include <GL/glew.h>

// GLFW, window handler
#include <GLFW/glfw3.h>

#include <rendering/ShaderProgram.hpp>
#include <rendering/textures.hpp>
#include <math/randomized.hpp>
#include <common/Rotator.hpp>

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

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
    //glfwWindowHint(GLFW_RESIZABLE, GL_FALSE); //Should be true?

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

    /**************** Callback functions ****************/
    glfwSetKeyCallback(window, key_callback);


    /***************** Declare variables ****************/
    GLfloat vertices[] = {
            0.5f,  0.5f, 0.0f,  // Top Right
            0.5f, -0.5f, 0.0f,  // Bottom Right
            -0.5f, -0.5f, 0.0f,  // Bottom Left
            -0.5f,  0.5f, 0.0f   // Top Left
    };
    GLuint indices[] = {
            0, 1, 3,    // First triangle
            1, 2, 3     // Second triangle
    };

    /****************** EBOs ****************************/
    GLuint temp_ebo;
    glGenBuffers(1, &temp_ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, temp_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    /****************** VBOs ****************************/

    GLuint temp_vbo;
    glGenBuffers(1, &temp_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, temp_vbo); //Can only bind one object to each buffer type
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    // How much will it move: not = GL_STATIC_DRAW, a lot = GL_DYNAMIC_DRAW, every render = GL_STREAM_DRAW


    /****************** VAOs ****************************/
    GLuint temp_vao;
    glGenVertexArrays(1, &temp_vao);
    glBindVertexArray(temp_vao);
    // Bind correct VBO and specify location (0)
    glBindBuffer(GL_ARRAY_BUFFER, temp_vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, temp_ebo);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0); //Vertices
    // Normals?

    // Enable all VAOs
    glEnableVertexAttribArray(0);

    // Unbind VAO
    glBindVertexArray(0);

    /****************** FBOs ****************************/



    /****************** Shaders *************************/
    // Declare shader and bind it
    ShaderProgram tempShader("../shaders/template.vert", "", "", "", "../shaders/template.frag");
    tempShader();


    /****************** Uniform variables ***************/




    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        // Check events
        glfwPollEvents();

        // Update window size
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);

        // OpenGL settings
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // GL_FILL or GL_LINE

        /********** Render stuff ***************/
        // Bind VAO
        glBindVertexArray(temp_vao);

        //glDrawArrays(GL_TRIANGLES, 0, 3);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        // Unbind VAO
        glBindVertexArray(0);

        // Swap front and back buffers
        glfwSwapBuffers(window);
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    // Close window on ESC
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
}