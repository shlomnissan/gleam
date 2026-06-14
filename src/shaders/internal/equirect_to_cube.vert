#version 410 core

uniform mat3 u_FaceBasis;

out vec3 v_TexDir;

void main() {
    const vec2 positions[] = vec2[](
        vec2(-1.0, -1.0),
        vec2( 3.0, -1.0),
        vec2(-1.0,  3.0)
    );

    v_TexDir = u_FaceBasis * vec3(positions[gl_VertexID], 1.0);

    gl_Position = vec4(positions[gl_VertexID], 0.0, 1.0);
}
