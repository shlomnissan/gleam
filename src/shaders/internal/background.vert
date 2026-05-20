#version 410 core

out vec2 v_TexCoords;

void main() {
    vec2 positions[] = vec2[](
        vec2(-1.0, -1.0),
        vec2( 3.0, -1.0),
        vec2(-1.0,  3.0)
    );

    vec2 uvs[] = vec2[](
        vec2(0.0, 0.0),
        vec2(2.0, 0.0),
        vec2(0.0, 2.0)
    );

    v_TexCoords = uvs[gl_VertexID];
    // z = 1.0 puts depth at the far plane so opaque fragments occlude the background
    gl_Position = vec4(positions[gl_VertexID], 1.0, 1.0);
}