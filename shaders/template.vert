#version 330 core

layout (location = 0) in vec3 position;
//layout (location = 1) in vec3 color;
layout (location = 1) in vec2 texCoord;

//out vec3 vColor;
out vec2 vTexCoord;

uniform mat4 MV;
uniform mat4 P;

void main()
{

    //vColor = color;
    vTexCoord = vec2(texCoord.x, 1.0f - texCoord.y);

    gl_Position = P * MV * vec4(position, 1.0f);

}

