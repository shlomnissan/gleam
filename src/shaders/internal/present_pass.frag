#version 410 core

layout (location = 0) out vec4 v_FragColor;

in vec2 v_TexCoord;

uniform sampler2D u_ResolvedTexture;

void main() {
    v_FragColor = texture(u_ResolvedTexture, v_TexCoord);
}