#version 330 core

uniform sampler2D u_texture_0;
uniform vec4 u_color;

out vec4 fragColor;

in vec2 texCoord;
in vec3 normal;
in vec3 fragPos;

void main()
{
    vec3 lightDir = normalize(vec3(0.1, 1.0, 0.1));

    vec4 color = texture(u_texture_0, texCoord) * u_color;

    float diff = max(dot(lightDir, normal.xyz), 0.0);

    vec3 ambient = color.rgb * 0.4;

    fragColor = vec4(pow(ambient + diff * 0.01, vec3(1.0/2.2)), color.a);
}