#version 330 core

out vec4 fragColor;

in vec2 texCoord;
in vec3 normal;
in vec3 color;
uniform sampler2D u_texture;

void main()
{
    fragColor = vec4(vec3((texCoord.y + 0.15) * 0.65) + vec3(0.0, 0.05, 0.1), 1.0);
}