#version 330 core

layout (location = 0) in vec4 aVertex;

uniform mat4 u_projection;
uniform mat4 u_view;
uniform mat4 u_model;

out vec2 texCoord;

void main() {
   texCoord = aVertex.zw;
   gl_Position = u_projection * u_view * u_model * vec4(aVertex.x, -aVertex.y, 0.0, 1.0);
}