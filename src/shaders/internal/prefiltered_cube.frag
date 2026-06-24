#version 410 core

layout(location = 0) out vec4 o_FragColor;

in vec3 v_TexDir;

uniform samplerCube u_EnvironmentMap;

uniform float u_Roughness;

const float PI = 3.14159265358979;

const uint kSampleCount = 1024u;

const float kRadianceClamp = 10.0; // cap per-sample radiance (firefly control)
const float kSourceMipBias = 2.0;  // extra source mips at roughness 1.0, scaled by roughness

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

float distributionGGX(vec3 N, vec3 H, float roughness) {
    float a = roughness * roughness;
    float a2 = a * a;
    float n_dot_h = max(dot(N, H), 0.0);
    float denom = n_dot_h * n_dot_h * (a2 - 1.0) + 1.0;
    return a2 / (PI * denom * denom);
}

void main() {
    vec3 N = normalize(v_TexDir);
    vec3 V = N;

    float resolution = float(textureSize(u_EnvironmentMap, 0).x);
    float sa_texel = 4.0 * PI / (6.0 * resolution * resolution);

    vec3 prefiltered = vec3(0.0);
    float total_weight = 0.0;

    for (uint i = 0u; i < kSampleCount; ++i) {
        vec2 xi = hammersley(i, kSampleCount);
        vec3 H = importanceSampleGGX(xi, N, u_Roughness);
        vec3 L = normalize(2.0 * dot(V, H) * H - V);

        float n_dot_l = max(dot(N, L), 0.0);
        if (n_dot_l > 0.0) {
            float D = distributionGGX(N, H, u_Roughness);
            float n_dot_h = max(dot(N, H), 0.0);
            float h_dot_v = max(dot(H, V), 0.0);
            float pdf = (D * n_dot_h / (4.0 * h_dot_v)) + 0.0001;

            float sa_sample = 1.0 / (float(kSampleCount) * pdf + 0.0001);

            // Scaled mip bias pre-blurs the source more as roughness grows, so
            // bright lights converge to a soft glow (not a sparkly spot) while
            // near-mirror reflections stay sharp.
            float mip = u_Roughness == 0.0 ? 0.0
                      : 0.5 * log2(sa_sample / sa_texel) + kSourceMipBias * u_Roughness;

            vec3 radiance = min(textureLod(u_EnvironmentMap, L, mip).rgb, vec3(kRadianceClamp));
            prefiltered += radiance * n_dot_l;
            total_weight += n_dot_l;
        }
    }

    prefiltered /= total_weight;

    o_FragColor = vec4(prefiltered, 1.0);
}
