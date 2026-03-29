#version 410 core

layout(location = 0) in vec3 a_Position;

layout(std140) uniform ub_Camera {
    mat4 u_Projection;
    mat4 u_View;
};

out vec3 v_TexDir;

void main() {
    v_TexDir = a_Position;
    // set z = w so depth is always 1.0 (far plane) after perspective divide
    gl_Position = (u_Projection * mat4(mat3(u_View)) * vec4(a_Position, 1.0)).xyww;
}