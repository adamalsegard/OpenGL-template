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
void renderSphere();

std::chrono::duration<double> second_accumulator;
unsigned int frames_last_second;
float fov = 45.0f;
bool shaderIsDirty = false;

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

    int nrRows    = 7;
    int nrColumns = 7;
    float spacing = 2.5;
    

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
    /*int tex_w, tex_h;
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
    glBindTexture(GL_TEXTURE_2D, 0);*/

    /*** TEX2 ***/
    /*// Load image from file
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
    glBindTexture(GL_TEXTURE_2D, 0);*/


    unsigned int roughnessTex   = loadTextureSOIL("../textures/Iron-Rusted/rustediron2_roughness.png");
    unsigned int normalTex      = loadTextureSOIL("../textures/Iron-Rusted/rustediron2_normal.png");
    unsigned int metallicTex    = loadTextureSOIL("../textures/Iron-Rusted/rustediron2_metallic.png");
    unsigned int albedoTex      = loadTextureSOIL("../textures/Iron-Rusted/rustediron2_basecolor.png");
    unsigned int aoTex          = loadTextureSOIL("../textures/Iron-Rusted/rustediron2_ao.png"); // TODO: Change!




    /****************** FBOs ****************************/



    /****************** Shaders *************************/
    // Declare shader and bind it
    ShaderProgram pbrShader("../shaders/pbr.vert", "../shaders/pbr.frag");

    pbrShader.M_Loc = glGetUniformLocation(pbrShader, "M");
    pbrShader.V_Loc = glGetUniformLocation(pbrShader, "V");
    pbrShader.P_Loc = glGetUniformLocation(pbrShader, "P");

    pbrShader.albedo_Loc = glGetUniformLocation(pbrShader, "albedoMap");
    pbrShader.metallic_Loc = glGetUniformLocation(pbrShader, "metallicMap");
    pbrShader.roughness_Loc = glGetUniformLocation(pbrShader, "roughnessMap");
    pbrShader.ao_Loc = glGetUniformLocation(pbrShader, "aoMap");
    pbrShader.normal_Loc = glGetUniformLocation(pbrShader, "normalMap");
    pbrShader.camPos_Loc = glGetUniformLocation(pbrShader, "camPos");

    /****************** Uniform variables ***************/
    glm::mat4 MV, M, V, P;
    glm::vec3 lDir;

    glm::vec3 albedo = glm::vec3(0.5f, 0.0f, 0.0f);
    float ao = 1.0f;
    float roughness;
    float metallic;

    // lights
    glm::vec3 lightPositions[] = {
            glm::vec3(-10.0f,  10.0f, 10.0f),
            glm::vec3( 10.0f,  10.0f, 10.0f),
            glm::vec3(-10.0f, -10.0f, 10.0f),
            glm::vec3( 10.0f, -10.0f, 10.0f),
    };
    glm::vec3 lightColors[] = {
            glm::vec3(300.0f, 300.0f, 300.0f),
            glm::vec3(300.0f, 300.0f, 300.0f),
            glm::vec3(300.0f, 300.0f, 300.0f),
            glm::vec3(300.0f, 300.0f, 300.0f)
    };

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

        P = glm::perspective(glm::radians(fov), (float)width/(float)height, 0.1f, 100.0f);

        // OpenGL settings
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // GL_FILL or GL_LINE

        /********** Render stuff ***************/
        // Bind Framebuffer

        // Bind shader
        if (shaderIsDirty) {
            pbrShader.Reload();
            shaderIsDirty = false;
        }

        pbrShader();

        // Send static uniforms
        glUniformMatrix4fv(pbrShader.V_Loc, 1, GL_FALSE, glm::value_ptr(V));
        glUniformMatrix4fv(pbrShader.P_Loc, 1, GL_FALSE, glm::value_ptr(P));
        //glUniform3fv(pbrShader.albedo_Loc, 1, glm::value_ptr(albedo));
        //glUniform1f(pbrShader.ao_Loc, ao);
        glUniform3fv(pbrShader.camPos_Loc, 1, glm::value_ptr(cameraPos));

        // Bind textures
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, albedoTex);
        glUniform1i(pbrShader.albedo_Loc, 0);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, normalTex);
        glUniform1i(pbrShader.normal_Loc, 1);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, metallicTex);
        glUniform1i(pbrShader.metallic_Loc, 2);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, roughnessTex);
        glUniform1i(pbrShader.roughness_Loc, 3);
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, aoTex);
        glUniform1i(pbrShader.ao_Loc, 4);

        // render rows*column number of spheres with varying metallic/roughness values scaled by rows and columns respectively
        glm::mat4 model = glm::mat4(1.0f);
        for (int row = 0; row < nrRows; ++row)
        {
            metallic = (float)row / (float)nrRows;
            for (int col = 0; col < nrColumns; ++col)
            {
                // we clamp the roughness to 0.025 - 1.0 as perfectly smooth surfaces (roughness of 0.0) tend to look a bit off
                // on direct lighting.
                roughness = glm::clamp((float)col / (float)nrColumns, 0.05f, 1.0f);

                M = glm::mat4(1.0f);
                M = glm::translate(model, glm::vec3(
                        (col - (nrColumns / 2)) * spacing,
                        (row - (nrRows / 2)) * spacing,
                        0.0f
                ));

                // Send variable uniforms
                glUniformMatrix4fv(pbrShader.M_Loc, 1, GL_FALSE, glm::value_ptr(M));
                //glUniform1f(pbrShader.metallic_Loc, metallic);
                //glUniform1f(pbrShader.roughness_Loc, roughness);
                renderSphere();
            }
        }



        // render light source (simply re-render sphere at light positions)
        // this looks a bit off as we use the same shader, but it'll make their positions obvious and
        // keeps the codeprint small.
        for (unsigned int i = 0; i < sizeof(lightPositions) / sizeof(lightPositions[0]); ++i)
        {
            glm::vec3 newPos = lightPositions[i] + glm::vec3(sin(glfwGetTime() * 5.0) * 5.0, 0.0, 0.0);
            newPos = lightPositions[i];

            M = glm::mat4(1.0f);
            M = glm::translate(model, newPos);

            // Send variable uniforms
            glUniformMatrix4fv(pbrShader.M_Loc, 1, GL_FALSE, glm::value_ptr(M));
            std::string lightPos_Loc = "lightPos[" + std::to_string(i) + "]";
            std::string lightColor_Loc = "lightColor[" + std::to_string(i) + "]";
            glUniform3fv(glGetUniformLocation(pbrShader, lightPos_Loc.c_str()), 1, glm::value_ptr(newPos));
            glUniform3fv(glGetUniformLocation(pbrShader, lightColor_Loc.c_str()), 1, glm::value_ptr(lightColors[i]));

            renderSphere();
        }

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
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
    if(key == GLFW_KEY_F5 && action == GLFW_PRESS) {
        shaderIsDirty = true;
    }

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


// renders (and builds at first invocation) a sphere
// -------------------------------------------------
unsigned int sphereVAO = 0;
unsigned int indexCount;
void renderSphere()
{
    if (sphereVAO == 0)
    {
        glGenVertexArrays(1, &sphereVAO);

        unsigned int vbo, ebo;
        glGenBuffers(1, &vbo);
        glGenBuffers(1, &ebo);

        std::vector<glm::vec3> positions;
        std::vector<glm::vec2> uv;
        std::vector<glm::vec3> normals;
        std::vector<unsigned int> indices;

        const unsigned int X_SEGMENTS = 64;
        const unsigned int Y_SEGMENTS = 64;
        const float PI = 3.14159265359;
        for (unsigned int y = 0; y <= Y_SEGMENTS; ++y)
        {
            for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
            {
                float xSegment = (float)x / (float)X_SEGMENTS;
                float ySegment = (float)y / (float)Y_SEGMENTS;
                float xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
                float yPos = std::cos(ySegment * PI);
                float zPos = std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);

                positions.push_back(glm::vec3(xPos, yPos, zPos));
                uv.push_back(glm::vec2(xSegment, ySegment));
                normals.push_back(glm::vec3(xPos, yPos, zPos));
            }
        }

        bool oddRow = false;
        for (int y = 0; y < Y_SEGMENTS; ++y)
        {
            if (!oddRow) // even rows: y == 0, y == 2; and so on
            {
                for (int x = 0; x <= X_SEGMENTS; ++x)
                {
                    indices.push_back(y       * (X_SEGMENTS + 1) + x);
                    indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
                }
            }
            else
            {
                for (int x = X_SEGMENTS; x >= 0; --x)
                {
                    indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
                    indices.push_back(y       * (X_SEGMENTS + 1) + x);
                }
            }
            oddRow = !oddRow;
        }
        indexCount = indices.size();

        std::vector<float> data;
        for (int i = 0; i < positions.size(); ++i)
        {
            data.push_back(positions[i].x);
            data.push_back(positions[i].y);
            data.push_back(positions[i].z);
            if (uv.size() > 0)
            {
                data.push_back(uv[i].x);
                data.push_back(uv[i].y);
            }
            if (normals.size() > 0)
            {
                data.push_back(normals[i].x);
                data.push_back(normals[i].y);
                data.push_back(normals[i].z);
            }
        }
        glBindVertexArray(sphereVAO);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), &data[0], GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
        float stride = (3 + 2 + 3) * sizeof(float);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(5 * sizeof(float)));
    }

    glBindVertexArray(sphereVAO);
    glDrawElements(GL_TRIANGLE_STRIP, indexCount, GL_UNSIGNED_INT, 0);
}