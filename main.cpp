#include <iostream>
#include <chrono>
#include <iomanip>

// GLM, header-only
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// OpenGL Extension Wrangler, must be included before GLFW
#include <GL/glew.h>

// GLFW, window handler
#include <GLFW/glfw3.h>

#include <rendering/ShaderProgram.hpp>
#include <rendering/TextureManager.hpp>
#include <SOIL.h>
#include <math/randomized.hpp>
#include <common/Navigation.hpp>
#include <sstream>

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

void setWindowFPS(GLFWwindow *window, float fps);

std::chrono::duration<double> second_accumulator;
unsigned int frames_last_second;
float fov = 45.0f;

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
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

    // Open window with GLFW
    //window = glfwCreateWindow(800, 600, "OpenGL template", NULL, NULL);
    window = glfwCreateWindow(1600, 1600, "OpenGL template", NULL, NULL);
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

    // Hide cursor and capture it (FPS-game)
    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Init size
    int width, height;

    /**************** OpenGL functions ******************/
    GLint nrAttributes;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
    std::cout << "Maximum nr of vertex attributes supported: " << nrAttributes << std::endl;

    glEnable(GL_DEPTH_TEST);
    //glDepthFunc(GL_LESS);
    //glEnable(GL_CULL_FACE);
    //glCullFace(GL_BACK);
    //glClearColor(0.0,0.1,0.2,1);

    /**************** Callback functions ****************/
    glfwSetKeyCallback(window, key_callback);
    glfwSetScrollCallback(window, scroll_callback);

    /***************** Declare variables ****************/
    /*GLfloat vertices[] = { // Square
            // Positions (XYZ)    // Colors (RGB)     // Texture Coords (ST)
            0.5f,  0.5f, 0.0f,    1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // Top Right
            0.5f, -0.5f, 0.0f,    0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // Bottom Right
            -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // Bottom Left
            -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // Top Left
    };*/
    GLfloat vertices[] = { // Cube
            -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
            0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
            0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
            0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
            -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
            0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
            0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
            0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
            -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

            -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
            -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
            -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

            0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
            0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
            0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
            0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
            0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
            0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
            0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
            0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
            0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

            -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
            0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
            0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
            0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
            -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
            -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
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
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
    // How much will it move: not = GL_STATIC_DRAW, a lot = GL_DYNAMIC_DRAW, every render = GL_STREAM_DRAW


    /****************** VAOs ****************************/
    GLuint temp_vao;
    glGenVertexArrays(1, &temp_vao);
    glBindVertexArray(temp_vao);
    // Bind correct VBO and specify location (0)
    glBindBuffer(GL_ARRAY_BUFFER, temp_vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, temp_ebo);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0); //Positions
    //glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat))); // Colors
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat))); //Texture Coords

    // Enable all VAOs
    glEnableVertexAttribArray(0);
    //glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(1);

    // Unbind VAO
    glBindVertexArray(0);

    /****************** Textures ************************/

    // Load image from file
    int tex_w, tex_h;
    unsigned char* tex_image = SOIL_load_image("../textures/container.jpg", &tex_w, &tex_h, 0, SOIL_LOAD_RGB);

    // Generate texture object
    GLuint texture1;
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Bind texture from image
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tex_w, tex_h, 0, GL_RGB, GL_UNSIGNED_BYTE, tex_image);
    glGenerateMipmap(GL_TEXTURE_2D); // Automatically generate mipmaps (instead of changing par 2 above)

    // Free image memory
    SOIL_free_image_data(tex_image);
    glBindTexture(GL_TEXTURE_2D, 0);

    /*** TEX2 ***/
    // Load image from file
    tex_image = SOIL_load_image("../textures/awesomeface.png", &tex_w, &tex_h, 0, SOIL_LOAD_RGB);

    // Generate texture object
    GLuint texture2;
    glGenTextures(1, &texture2);
    glBindTexture(GL_TEXTURE_2D, texture2);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Bind texture from image
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tex_w, tex_h, 0, GL_RGB, GL_UNSIGNED_BYTE, tex_image);
    glGenerateMipmap(GL_TEXTURE_2D); // Automatically generate mipmaps (instead of changing par 2 above)

    // Free image memory
    SOIL_free_image_data(tex_image);
    glBindTexture(GL_TEXTURE_2D, 0);


    /****************** FBOs ****************************/



    /****************** Shaders *************************/
    // Declare shader and bind it
    ShaderProgram tempShader("../shaders/template.vert", "", "", "", "../shaders/template.frag");

    tempShader.MV_Loc = glGetUniformLocation(tempShader, "MV");
    tempShader.P_Loc = glGetUniformLocation(tempShader, "P");

    /****************** Uniform variables ***************/
    glm::mat4 MV, V, P;
    glm::vec3 lDir;
    glm::mat4 M = glm::mat4(1.0f);


    /******************* Other Stuff ********************/
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

        double dt_s = 1e-3 * dt_ms.count();
#ifdef MY_DEBUG
        std::cout << "Seconds: " << dt_s << "\n";
#endif
        //dt_s = std::min(dt_s, 1.0f / 60.0f);
        /*----------------------------------------------------------------------------------------*/

        // Check events
        glfwPollEvents();
        rotator.poll(window);
        trans.poll(window);
        //printf("phi = %6.2f, theta = %6.2f\n", rotator.phi, rotator.theta);

        // Update window size
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);

        // Update camera
        glm::vec3 cameraFront;
        cameraFront.x = cos(glm::radians(rotator.pitch)) * cos(glm::radians(rotator.yaw));
        cameraFront.y = sin(glm::radians(rotator.pitch));
        cameraFront.z = cos(glm::radians(rotator.pitch)) * sin(glm::radians(rotator.yaw));

        glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
        glm::vec3 cameraRight = glm::normalize(glm::cross(cameraFront, cameraUp));

        glm::vec3 cameraPos = trans.horizontal * cameraRight + trans.zoom * cameraFront;
        cameraPos.y = 0.0f;
        V = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

        MV = V*M;
        P = glm::perspective(glm::radians(fov), (float)width/(float)height, 0.1f, 100.0f);

        //Calculate light direction
        lDir = glm::vec3(1.0f, -1.0f, 1.0f);

        // OpenGL settings
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // GL_FILL or GL_LINE

        /********** Render stuff ***************/
        // Bind Framebuffer

        // Bind shader
        tempShader();

        // Send Uniforms
        glUniformMatrix4fv(tempShader.MV_Loc, 1, GL_FALSE, glm::value_ptr(MV));
        glUniformMatrix4fv(tempShader.P_Loc, 1, GL_FALSE, glm::value_ptr(P));

        // Bind textures
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        glUniform1i(glGetUniformLocation(tempShader, "ourTexture1"), 0);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);
        glUniform1i(glGetUniformLocation(tempShader, "ourTexture2"), 1);

        // Bind VAO
        glBindVertexArray(temp_vao);

        // Draw elements
        glDrawArrays(GL_TRIANGLES, 0, 36);
        //glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

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

    // Properly de-allocate all resources once they've outlived their purpose
    glDeleteVertexArrays(1, &temp_vao);
    glDeleteBuffers(1, &temp_vbo);
    glDeleteBuffers(1, &temp_ebo);

    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) {
    // Close window on ESC
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    if(fov >= 1.0f && fov <= 120.0f) {
        fov -= yoffset * 5.0f;
    }
    if(fov <= 1.0f) {
        fov = 1.0f;
    }

    if(fov >= 120.0f) {
        fov = 120.0f;
    }

}

void setWindowFPS(GLFWwindow *window, float fps) {
    std::stringstream ss;
    ss << "FPS: " << fps;

    glfwSetWindowTitle(window, ss.str().c_str());
}