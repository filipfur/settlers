#version 330 core

layout (location = 0) in vec4 aVertex;

uniform mat4 u_camera;
uniform mat4 u_model;

out vec2 texCoord;
out vec2 position;

void main() {
   texCoord = aVertex.zw;
   position = vec2(aVertex.x, -aVertex.y);
   vec3 p0 = vec3(u_model * vec4(position, 0.0, 1.0));
   gl_Position = u_camera * vec4(p0, 1.0);
}