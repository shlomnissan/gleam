#version 410 core

layout (location = 0) out vec4 v_FragColor;

in vec2 v_TexCoord;

void main() {
    v_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
}