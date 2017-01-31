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
#include <sstream>

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

void setWindowFPS(GLFWwindow *window, float fps);

std::chrono::duration<double> second_accumulator;
unsigned int frames_last_second;

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
    //glfwWindowHint(GLFW_RESIZABLE, GL_FALSE); //Should be GL_TRUE!

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

    //Generate rotator and translator
    MouseRotator rotator;
    rotator.init(window);
    KeyTranslator trans;
    trans.init(window);

    // Init size
    int width, height;

    /**************** OpenGL functions ******************/
    //glEnable(GL_DEPTH_TEST);
    //glDepthFunc(GL_LESS);
    //glEnable(GL_CULL_FACE);
    //glCullFace(GL_BACK);
    //glClearColor(0.0,0.1,0.2,1);

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

    /****************** Textures ************************/



    /****************** FBOs ****************************/



    /****************** Shaders *************************/
    // Declare shader and bind it
    ShaderProgram tempShader("../shaders/template.vert", "", "", "", "../shaders/template.frag");
    tempShader();


    /****************** Uniform variables ***************/
    glm::mat4 MV, P;
    glm::vec3 lDir;
    glm::mat4 M = glm::mat4(1.0f);
    float radius = 0.1f;


    // FPS
    std::chrono::high_resolution_clock::time_point tp_last = std::chrono::high_resolution_clock::now();
    second_accumulator = std::chrono::duration<double>(0);
    frames_last_second = 0;


    /******************* RENDER LOOP *********************/
    while (!glfwWindowShouldClose(window))
    {
        /*------------------Update clock and FPS---------------------------------------------*/
        std::chrono::high_resolution_clock::time_point tp_now = std::chrono::high_resolution_clock::now();
        std::chrono::high_resolution_clock::duration delta_time = tp_now - tp_last;
        tp_last = tp_now;

        std::chrono::milliseconds dt_ms = std::chrono::duration_cast<std::chrono::milliseconds>(delta_time);

        float dt_s = 1e-3 * dt_ms.count();
#ifdef MY_DEBUG
        std::cout << "Seconds: " << dt_s << "\n";
#endif
        //dt_s = std::min(dt_s, 1.0f / 60.0f);
        /*----------------------------------------------------------------------------------------*/

        // Check events
        glfwPollEvents();
        rotator.poll(window);
        trans.poll(window);

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

        /*---------------------- FPS DISPLAY HANDLING ------------------------------------*/
        ++frames_last_second;
        second_accumulator += delta_time;
        if (second_accumulator.count() >= 1.0) {
            float newFPS = static_cast<float>( frames_last_second / second_accumulator.count());
            setWindowFPS(window, newFPS);
            frames_last_second = 0;
            second_accumulator = std::chrono::duration<double>(0);
        }
        /*--------------------------------------------------------------------------------*/
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

void setWindowFPS(GLFWwindow *window, float fps) {
    std::stringstream ss;
    ss << "FPS: " << fps;

    glfwSetWindowTitle(window, ss.str().c_str());
}