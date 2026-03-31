#version 410 core

layout(location = 0) in vec3 a_Position;

layout(std140) uniform ub_Camera {
    mat4 u_Projection;
    mat4 u_View;
};

out vec3 v_TexDir;

void main() {
    v_TexDir = a_Position;
    // Reverse-z: set z = -w so depth is always 0.0 (far plane) after perspective divide
    vec4 pos = u_Projection * mat4(mat3(u_View)) * vec4(a_Position, 1.0);
    gl_Position = vec4(pos.xy, -pos.w, pos.w);
}