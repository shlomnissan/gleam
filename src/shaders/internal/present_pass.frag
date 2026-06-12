#version 410 core

layout (location = 0) out vec4 v_FragColor;

in vec2 v_TexCoords;

uniform int u_ToneMapping; // 0 = None, 1 = ACESFilmic
uniform float u_Exposure;

uniform sampler2D u_ResolvedTexture;

vec3 linearToSRGB(vec3 c) {
    vec3 lo = c * 12.92;
    vec3 hi = 1.055 * pow(max(c, 0.0), vec3(1.0 / 2.4)) - 0.055;
    return mix(hi, lo, vec3(lessThanEqual(c, vec3(0.0031308))));
}

vec3 acesFilmic(vec3 x) {
    const float a = 2.51;
    const float b = 0.03;
    const float c = 2.43;
    const float d = 0.59;
    const float e = 0.14;
    return clamp((x * (a * x + b)) / (x * (c * x + d) + e), 0.0, 1.0);
}

vec3 toneMap(vec3 color) {
    if (u_ToneMapping == 1) {
        return acesFilmic(color);
    }
    return clamp(color, 0.0, 1.0);
}

void main() {
    vec3 color = texture(u_ResolvedTexture, v_TexCoords).rgb;
    color *= u_Exposure;
    color = toneMap(color);
    color = linearToSRGB(color);
    v_FragColor = vec4(color, 1.0);
}
