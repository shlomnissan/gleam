#version 410 core

layout(location = 0) out vec4 v_FragColor;

in vec3 v_TexDir;

uniform samplerCube u_EnvironmentMap;

const float PI = 3.14159265358979;

const float kSampleLod = 1.0;

void main() {
    vec3 N = normalize(v_TexDir);

    // Tangent basis around N. Guard the pole: at the +Y/-Y face centers N is
    // exactly (0,±1,0), and cross((0,1,0), N) collapses to zero -> NaN texel.
    vec3 up = abs(N.y) < 0.999 ? vec3(0.0, 1.0, 0.0) : vec3(1.0, 0.0, 0.0);
    vec3 right = normalize(cross(up, N));
    up = normalize(cross(N, right));

    vec3 irradiance = vec3(0.0);
    float num_samples = 0.0;

    const float sample_delta = 0.025;
    for (float phi = 0.0; phi < 2.0 * PI; phi += sample_delta) {
        for (float theta = 0.0; theta < 0.5 * PI; theta += sample_delta) {
            // tangent-space hemisphere direction -> world space
            vec3 tangent = vec3(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
            vec3 dir = tangent.x * right + tangent.y * up + tangent.z * N;

            // cos(theta): Lambert.  sin(theta): solid-angle weight of the ring.
            irradiance += textureLod(u_EnvironmentMap, dir, kSampleLod).rgb
                        * cos(theta) * sin(theta);
            num_samples += 1.0;
        }
    }

    irradiance = PI * irradiance / num_samples;

    v_FragColor = vec4(irradiance, 1.0);
}