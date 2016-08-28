#include <iostream>
#include <chrono>
#include <iomanip>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <sstream>

#ifdef _WIN32
#include "GL/glew.h"
#endif

#include <GLFW/glfw3.h>

#include "rendering/ShaderProgram.hpp"
#include "common/Rotator.hpp"

using std::cout;
using std::endl;


void setWindowFPS(GLFWwindow *window, float fps);

std::chrono::duration<double> second_accumulator;
unsigned int frames_last_second;

int main() {

    GLFWwindow *window;

    if (!glfwInit()) {
        exit(EXIT_FAILURE);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    //Open a window
    window = glfwCreateWindow(640, 480, "The coolest name ever", NULL, NULL);
    if (!window) {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    //Generate rotator
    MouseRotator rotator;
    rotator.init(window);
    KeyTranslator trans;
    trans.init(window);

    //Set the GLFW-context the current window
    glfwMakeContextCurrent(window);
    cout << glGetString(GL_VERSION) << "\n";

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glDepthFunc(GL_LESS);

    // VSync: enable = 1, disable = 0
    glfwSwapInterval(0);

#ifdef _WIN32
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
        /* Problem: glewInit failed, something is seriously wrong. */
        cout << "GLEW init error: " << glewGetErrorString(err) << "\n";
        return -1;
    }
#endif


//---------- INITIALIZE STUFF (like VBOs, VAO, Shaders)




    // Declare which shader to use and bind it
    ShaderProgram templateShader("../shaders/template.vert", "", "", "", "../shaders/template.frag");
    templateShader();


    //Declare uniform locations
    GLint MV_Loc, P_Loc, lDir_Loc = -1;
    MV_Loc = glGetUniformLocation(templateShader, "MV");
    P_Loc = glGetUniformLocation(templateShader, "P");
    lDir_Loc = glGetUniformLocation(templateShader, "lDir");
    glm::mat4 MV, P;
    glm::vec3 lDir;
    glm::mat4 M = glm::mat4(1.0f);

    //Specify which pixels to draw to
    int width, height;


    std::chrono::high_resolution_clock::time_point tp_last = std::chrono::high_resolution_clock::now();
    second_accumulator = std::chrono::duration<double>(0);
    frames_last_second = 0;

    while (!glfwWindowShouldClose(window)) {
        std::chrono::high_resolution_clock::time_point tp_now = std::chrono::high_resolution_clock::now();
        std::chrono::high_resolution_clock::duration delta_time = tp_now - tp_last;
        tp_last = tp_now;

        std::chrono::milliseconds dt_ms = std::chrono::duration_cast<std::chrono::milliseconds>(delta_time);

        float dt_s = 1e-3 * dt_ms.count();
#ifdef MY_DEBUG
        cout << "Seconds: " << dt_s << "\n";
#endif
        //dt_s = std::min(dt_s, 1.0f / 60);

        // Update window size
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);


        // Get mouse and key input
        rotator.poll(window);
        trans.poll(window);
        //printf("phi = %6.2f, theta = %6.2f\n", rotator.phi, rotator.theta);
        glm::mat4 VRotX = glm::rotate(M, rotator.phi, glm::vec3(0.0f, 1.0f, 0.0f)); //Rotation about y-axis
        glm::mat4 VRotY = glm::rotate(M, rotator.theta, glm::vec3(1.0f, 0.0f, 0.0f)); //Rotation about x-axis
        glm::mat4 VTrans = glm::translate(M, glm::vec3(trans.horizontal, 0.0f, trans.zoom));

        glm::vec4 eye_position = VRotX * VRotY * glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);

        glm::mat4 V = VTrans * glm::lookAt(glm::vec3(eye_position), glm::vec3(0.0f, 0.0f, 0.0f),
                                           glm::vec3(0.0f, 1.0f, 0.0f));
        P = glm::perspectiveFov(50.0f, static_cast<float>(width), static_cast<float>(height), 0.1f, 100.0f);
        MV = V * M;

        //Calculate light direction
        lDir = glm::vec3(1.0f);


        //Send uniform variables
        glUniformMatrix4fv(MV_Loc, 1, GL_FALSE, &MV[0][0]);
        glUniformMatrix4fv(P_Loc, 1, GL_FALSE, &P[0][0]);
        glUniform3fv(lDir_Loc, 1, &lDir[0]);


        // Clear the buffers
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glCullFace(GL_BACK);

        //Send VAO to the GPU



        glfwSwapBuffers(window);
        ++frames_last_second;
        glfwPollEvents();

        if (glfwGetKey(window, GLFW_KEY_ESCAPE)) {
            glfwSetWindowShouldClose(window, 1);
        }

        /* FPS DISPLAY HANDLING */
        second_accumulator += delta_time;
        if (second_accumulator.count() >= 1.0) {
            float newFPS = static_cast<float>( frames_last_second / second_accumulator.count());
            setWindowFPS(window, newFPS);
            std::stringstream fpsString;
            fpsString << std::fixed << std::setprecision(0) << newFPS;
            frames_last_second = 0;
            second_accumulator = std::chrono::duration<double>(0);
        }
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}

void setWindowFPS(GLFWwindow *window, float fps) {
    std::stringstream ss;
    ss << "FPS: " << fps;

    glfwSetWindowTitle(window, ss.str().c_str());
}




