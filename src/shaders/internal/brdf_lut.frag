#version 410 core

layout(location = 0) out vec2 o_FragColor;

in vec2 v_TexCoords;

const float PI = 3.14159265358979;

const uint kSampleCount = 1024u;

float radicalInverseVdC(uint bits) {
    bits = (bits << 16u) | (bits >> 16u);
    bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
    bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
    bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
    bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
    return float(bits) * 2.3283064365386963e-10;
}

vec2 hammersley(uint i, uint n) {
    return vec2(float(i) / float(n), radicalInverseVdC(i));
}

vec3 importanceSampleGGX(vec2 xi, vec3 N, float roughness) {
    float a = roughness * roughness;

    float phi = 2.0 * PI * xi.x;
    float cos_theta = sqrt((1.0 - xi.y) / (1.0 + (a * a - 1.0) * xi.y));
    float sin_theta = sqrt(1.0 - cos_theta * cos_theta);

    vec3 H = vec3(cos(phi) * sin_theta, sin(phi) * sin_theta, cos_theta);

    vec3 up = abs(N.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);
    vec3 tangent = normalize(cross(up, N));
    vec3 bitangent = cross(N, tangent);

    return normalize(tangent * H.x + bitangent * H.y + N * H.z);
}

float geometrySchlickGGX(float n_dot_x, float roughness) {
    float k = (roughness * roughness) / 2.0;
    return n_dot_x / (n_dot_x * (1.0 - k) + k);
}

float geometrySmith(float n_dot_v, float n_dot_l, float roughness) {
    return geometrySchlickGGX(n_dot_v, roughness) * geometrySchlickGGX(n_dot_l, roughness);
}

vec2 integrateBRDF(float n_dot_v, float roughness) {
    vec3 V = vec3(sqrt(1.0 - n_dot_v * n_dot_v), 0.0, n_dot_v);
    vec3 N = vec3(0.0, 0.0, 1.0);

    float A = 0.0;
    float B = 0.0;

    for (uint i = 0u; i < kSampleCount; ++i) {
        vec2 xi = hammersley(i, kSampleCount);
        vec3 H = importanceSampleGGX(xi, N, roughness);
        vec3 L = normalize(2.0 * dot(V, H) * H - V);

        float n_dot_l = max(L.z, 0.0);
        float n_dot_h = max(H.z, 0.0);
        float v_dot_h = max(dot(V, H), 0.0);

        if (n_dot_l > 0.0) {
            float G = geometrySmith(n_dot_v, n_dot_l, roughness);
            float g_vis = (G * v_dot_h) / (n_dot_h * n_dot_v);
            float fc = pow(1.0 - v_dot_h, 5.0);

            A += (1.0 - fc) * g_vis;
            B += fc * g_vis;
        }
    }

    return vec2(A, B) / float(kSampleCount);
}

void main() {
    o_FragColor = integrateBRDF(v_TexCoords.x, v_TexCoords.y);
}
