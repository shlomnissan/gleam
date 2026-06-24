#version 410 core

layout(location = 0) out vec4 o_FragColor;

in vec2 v_TexCoords;

uniform sampler2D u_BackgroundTexture;

void main() {
    o_FragColor = vec4(texture(u_BackgroundTexture, v_TexCoords).rgb, 1.0);
}
