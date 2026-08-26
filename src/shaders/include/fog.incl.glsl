#ifdef USE_FOG

struct Fog {
    int Type; // 0 = linear, 1 = exponential
    vec3 Color;
    float Near;
    float Far;
    float Density;
};

uniform Fog u_Fog;

void applyFog(inout vec3 color, const in float depth) {
    float fog_factor = 0.0;
    if (u_Fog.Type == 0) {
        fog_factor = smoothstep(u_Fog.Near, u_Fog.Far, depth);
    }
    if (u_Fog.Type == 1) {
        fog_factor = 1.0 - exp(-u_Fog.Density * u_Fog.Density * depth * depth);
    }
    color = mix(color, u_Fog.Color, fog_factor);
}

#endif
