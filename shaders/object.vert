#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

uniform mat4 u_view;
uniform mat4 u_projection;
uniform mat4 u_model;
uniform float u_time;

out vec2 texCoord;
out vec3 normal;
out vec3 fragPos;

void main()
{
    texCoord = aTexCoords;
    mat3 normalMatrix = transpose(inverse(mat3(u_model)));
    normal = normalize(normalMatrix * aNormal);
    fragPos = vec3(u_model * vec4(aPos, 1.0));
    gl_Position = u_projection * u_view * vec4(fragPos, 1.0);
}