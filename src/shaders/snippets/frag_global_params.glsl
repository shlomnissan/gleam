/*

This fragment shader snippet defines global parameters, including the main
output color, varyings, and uniforms accessible within the fragment shader.

@out vec4 v_FragColor - Output color of the fragment
@varying vec2 v_TexCoord - Texture coordinates
@varying vec3 v_Normal - Normal vector (see frag_main_normal.glsl)
@varying vec3 v_ViewDir - View direction vector
@varying vec4 v_Position - Fragment position in view space
@uniform vec3 u_Color - Base color of the fragment
@uniform float u_Opacity - Fragment opacity

*/

layout (location = 0) out vec4 v_FragColor;

#ifdef USE_INSTANCING
    in vec3 v_InstanceColor;
#endif

#ifdef USE_VERTEX_COLOR
    in vec3 v_Color;
#endif

in float v_ViewDepth;
in vec2 v_TexCoord;
in vec3 v_Normal;
in vec3 v_ViewDir;
in vec4 v_Position;

uniform float u_Opacity;
uniform vec3 u_Color;

layout(std140) uniform ub_Camera {
    mat4 u_Projection;
    mat4 u_View;
};