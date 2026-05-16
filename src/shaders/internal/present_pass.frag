#version 410 core

layout (location = 0) out vec4 v_FragColor;

in vec2 v_TexCoords;

uniform sampler2D u_ResolvedTexture;

vec3 linearToSRGB(vec3 c) {
    vec3 lo = c * 12.92;
    vec3 hi = 1.055 * pow(max(c, 0.0), vec3(1.0 / 2.4)) - 0.055;
    return mix(hi, lo, vec3(lessThanEqual(c, vec3(0.0031308))));
}

void main() {
    vec3 color = texture(u_ResolvedTexture, v_TexCoords).rgb;
    color = linearToSRGB(color);
    v_FragColor = vec4(color, 1.0);
}