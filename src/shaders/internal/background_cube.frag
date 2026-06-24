#version 410 core

layout(location = 0) out vec4 o_FragColor;

in vec3 v_TexDir;

uniform samplerCube u_BackgroundCubeTexture;

void main() {
    o_FragColor = vec4(texture(u_BackgroundCubeTexture, v_TexDir).rgb, 1.0);
}
