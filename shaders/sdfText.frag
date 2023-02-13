 #version 330 core
 //precision mediump float;

out vec4 FragColor;

uniform vec4 u_color;
uniform sampler2D u_texture;

in vec2 texCoord;
in vec2 position;

 void main() {
    float sample = texture(u_texture, texCoord).r;
    float scale = 1.0 / fwidth(sample);
    float signedDistance = (sample - 0.5) * scale;
    float borderWidth = 0.2;//0.125;
    float color = clamp(signedDistance + 0.5, 0.0, 1.0);
    float alpha = clamp(signedDistance + 0.5 + scale * borderWidth, 0.0, 1.0);
    FragColor = vec4(u_color.rgb * color, u_color.a) * alpha;
    //FragColor += vec4(1, 0, 0, 1) * 0.5;
 }