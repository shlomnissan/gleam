#version 410 core

layout (location = 0) out vec4 v_FragColor;

in vec2 v_TexCoord;

uniform sampler2D u_ResolvedTexture;

void main() {
    vec3 color = texture(u_ResolvedTexture, v_TexCoord).rgb;
    color = pow(color, vec3(1.0 / 2.2));
    v_FragColor = vec4(color, 1.0);
}