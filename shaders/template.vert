#version 330 core

layout (location = 0) in vec3 position;

//uniform mat4 MV;
//uniform mat4 P;

void main()
{

    gl_Position = vec4(position, 1.0);
    //gl_Position = P * MV * vec4(position, 1.0f);

}

