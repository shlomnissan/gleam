#version 410 core

#pragma inject_attributes

in vec3 a_Position;

#ifdef USE_UV
    in vec2 a_TexCoord;
    uniform mat3 u_TextureTransform;
    out vec2 v_TexCoords;
#endif

#ifdef USE_VERTEX_COLOR
    in vec3 a_Color;
    out vec3 v_Color;
#endif

#ifdef USE_INSTANCING
    in mat4 a_InstanceTransform;
    in vec3 a_InstanceColor;
    out vec3 v_InstanceColor;
#endif

#ifdef USE_FOG
    out float v_ViewDepth;
#endif

uniform mat4 u_Model;

layout(std140) uniform ub_Camera {
    mat4 u_Projection;
    mat4 u_View;
};

void main() {
    mat4 model_view = u_View * u_Model;

    #ifdef USE_INSTANCING
        model_view *= a_InstanceTransform;
        v_InstanceColor = a_InstanceColor;
    #endif

    #ifdef USE_VERTEX_COLOR
        v_Color = a_Color;
    #endif

    #ifdef USE_UV
        v_TexCoords = (u_TextureTransform * vec3(a_TexCoord, 1.0)).xy;
    #endif

    vec4 position = model_view * vec4(a_Position, 1.0);

    #ifdef USE_FOG
        v_ViewDepth = -position.z;
    #endif

    gl_Position = u_Projection * position;
}
