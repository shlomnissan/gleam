#version 410 core

#pragma inject_attributes

#ifdef USE_UV
    in vec2 v_TexCoords;
#endif

uniform float u_Opacity;

#ifdef USE_ALPHA_TEST
    uniform float u_AlphaTest;
#endif

uniform sampler2D u_AlbedoMap;
uniform sampler2D u_AlphaMap;

void main() {
    #ifdef USE_ALPHA_TEST
        float opacity = u_Opacity;

        #ifdef USE_ALBEDO_MAP
            opacity *= texture(u_AlbedoMap, v_TexCoords).a;
        #endif

        #ifdef USE_ALPHA_MAP
            opacity *= texture(u_AlphaMap, v_TexCoords).r;
        #endif

        if (opacity < u_AlphaTest) discard;
    #endif
}
