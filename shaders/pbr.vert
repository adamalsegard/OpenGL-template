#version 330 core

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec2 inTexCoord;
layout (location = 2) in vec3 inNormal;

out vec2 vTexCoords;
out vec3 vWorldPos;
out vec3 vNormal;

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;

void main() {

    vTexCoords = inTexCoord;
    vWorldPos = vec3(M * vec4(inPosition, 1.0));
    vNormal = mat3(M) * inNormal;

    gl_Position = P * V * vec4(vWorldPos, 1.0f);
}