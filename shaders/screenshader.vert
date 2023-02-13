#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aColor;

out vec2 texCoord;
out vec3 normal;
out vec3 color;

void main()
{
    texCoord = aTexCoords;
    normal = aNormal;
    color = aColor;
    gl_Position = vec4(aPos, 1.0);
}