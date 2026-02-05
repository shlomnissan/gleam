#version 410 core

#extension GL_GOOGLE_include_directive : enable

#pragma inject_attributes

#include "snippets/vert_global_params.glsl"

#ifdef USE_NORMAL_MAP
    in vec4 a_Tangent;
    out mat3 v_TBN;
#endif

void main() {
    #include "snippets/vert_main_varyings.glsl"

    #ifdef USE_NORMAL_MAP
        vec3 tangent = normalize(mat3(u_ModelView) * a_Tangent.xyz);
        vec3 bitangent = normalize(cross(v_Normal, tangent)) * a_Tangent.w;
        v_TBN = mat3(tangent, bitangent, v_Normal);
    #endif

    gl_Position = u_Projection * v_Position;
}