#version 410 core

layout(location = 0) out vec4 v_FragColor;

in vec3 v_TexDir;

uniform sampler2D u_EquirectTexture;

const float PI = 3.14159265358979;

vec2 directionToEquirectUV(const in vec3 dir) {
    float u = atan(dir.z, dir.x) / (2.0 * PI) + 0.5;
    float v = asin(dir.y) / PI + 0.5;
    return vec2(u, v);
}

void main() {
    vec2 uv = directionToEquirectUV(normalize(v_TexDir));

    v_FragColor = vec4(textureLod(u_EquirectTexture, uv, 0.0).rgb, 1.0);
}
