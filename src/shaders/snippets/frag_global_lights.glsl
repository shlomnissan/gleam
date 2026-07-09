/*

This fragment shader snippet defines the Light struct, the ub_Lights uniform
block, and distance attenuation shared by all lit materials. It must be
included inside an #if NUM_LIGHTS > 0 block, after NUM_LIGHTS is defined.

@func float attenuation(const in float dist, const in Light light)
- @desc Computes physical inverse-square attenuation with an optional smooth
  cutoff that reaches zero at the light's range
- @param const in float dist - The distance from the light to the fragment
- @param const in Light light - The light whose range controls the cutoff
- @return float - Factor to multiply with the light's contribution

*/

struct Light {
    int Type; // 1 = directional, 2 = point, 3 = spot
    vec3 Color;
    vec3 Position;
    vec3 Direction;
    float ConeCos;
    float PenumbraCos;
    float Range;
    int ShadowLayerIndex;
    float ShadowBias;
    mat4 ShadowTransform;
};

#ifdef USE_SHADOW_MAPS
    uniform sampler2DArrayShadow u_ShadowMaps;
    uniform bool u_ReceiveShadow;
#endif

layout(std140) uniform ub_Lights {
    Light u_Lights[NUM_LIGHTS];
};

float attenuation(const in float dist, const in Light light) {
    float atten = 1.0 / max(dist * dist, 1e-4);
    if (light.Range > 0.0) {
        float window = clamp(1.0 - pow(dist / light.Range, 4.0), 0.0, 1.0);
        atten *= window * window;
    }
    return atten;
}
