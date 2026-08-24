#version 410 core

#pragma inject_attributes

in vec3 a_Position;

#ifdef USE_UV
    in vec2 a_TexCoord;
    uniform mat3 u_TextureTransform;
    out vec2 v_TexCoords;
#endif

#ifdef USE_FOG
    out float v_ViewDepth;
#endif

uniform mat4 u_Model;

layout(std140) uniform ub_Camera {
    mat4 u_Projection;
    mat4 u_View;
};

uniform float u_Rotation;
uniform vec2 u_Anchor;

bool isPerspectiveMatrix(in mat4 m) {
    return m[2][3] == -1.0;
}

void main() {
    mat4 model_view = u_View * u_Model;

    #ifdef USE_UV
        v_TexCoords = (u_TextureTransform * vec3(a_TexCoord, 1.0)).xy;
    #endif

    #ifdef USE_FOG
        v_ViewDepth = -(model_view * vec4(a_Position, 1.0)).z;
    #endif

    vec4 position = model_view[3];
    vec2 scale = vec2(length(u_Model[0].xyz), length(u_Model[1].xyz));

    #ifndef USE_SIZE_ATTENUATION
        bool is_perspective = isPerspectiveMatrix(u_Projection);
        if (is_perspective) scale *= -position.z;
    #endif

    vec2 offset = (a_Position.xy - (u_Anchor - vec2(0.5))) * scale;
    vec2 offset_with_rotation = vec2(0.0);
    offset_with_rotation.x = cos(u_Rotation) * offset.x - sin(u_Rotation) * offset.y;
    offset_with_rotation.y = sin(u_Rotation) * offset.x + cos(u_Rotation) * offset.y;

    position.xy += offset_with_rotation;

    gl_Position = u_Projection * position;
}
