# OpenGL-template
An OpenGL template to be used for new projects so I don't have to set up a new one every time!

###### Uses GLFW for window handling, GLEW for extensions and SOIL for texture files. 

Aims for modern OpenGL (core 3.3 with some implementations up to core 4.5).
Developed with CLion in Linux environment, but should build on Windows and MacOS with CMake.


#### Preperations
GLFW is built with project. GLEW and SOIL needs to be downloaded and compiled locally before linking in cmakelist.


#### Implemented
* Window handling (GLFW, GLEW, OpenGL 3.3 core)
* Shader program
* Texture manager (SOIL)
* Math library GLM


#### Coming up next: 
* Navigation class
* Complete TextureManager class

TODO: 
- Add init & render function for triangle, cube, sphere
- Add SendUniform function to Shader function
- Find materials at https://freepbr.com/materials/rusted-iron-pbr-metal-material-alt/
- Fix the Texture Manager class!
- Fix recompiling of shader