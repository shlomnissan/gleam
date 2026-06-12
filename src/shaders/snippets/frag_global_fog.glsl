/*

This fragment shader snippet applies a fog effect based on the fragment's
distance from the camera. It supports two fog types: linear and exponential,
but only one can be active at a time. To use the fog effect, call the
applyFog function with the color and depth as arguments.

@func void applyFog(inout vec3 color, const in float depth)
- @desc Applies the fog effect to the color based on the depth
- @param inout vec3 color - The color to be modified by the fog effect
- @param const in float depth - The depth of the fragment in view space
- @return void

*/

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
