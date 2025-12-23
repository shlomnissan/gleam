#version 410 core

#extension GL_GOOGLE_include_directive : enable

#pragma inject_attributes

#include "snippets/vert_global_params.glsl"
#include "snippets/utilities.glsl"

uniform float u_Rotation;
uniform vec2 u_Anchor;

void main() {
    #include "snippets/vert_main_varyings.glsl"

    vec4 position = u_ModelView[3];
    vec2 scale = vec2(length(u_Model[0].xyz), length(u_Model[1].xyz));

    bool is_perspective = isPerspectiveMatrix(u_Projection);
    if (is_perspective) {
        scale *= -position.z;
    }

    vec2 offset = (a_Position.xy - (u_Anchor - vec2(0.5))) * scale;
    vec2 offset_with_rotation = vec2(0.0);
    offset_with_rotation.x = cos(u_Rotation) * offset.x - sin(u_Rotation) * offset.y;
    offset_with_rotation.y = sin(u_Rotation) * offset.x + cos(u_Rotation) * offset.y;

    position.xy += offset_with_rotation;

    gl_Position = u_Projection * position;
}