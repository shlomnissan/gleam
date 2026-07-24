/*

This fragment shader snippet defines global parameters, including the main
output color, varyings, and uniforms accessible within the fragment shader.

@out vec4 o_FragColor - Output color of the fragment
@varying float v_ViewDepth - Depth of the fragment in view space
@varying vec2 v_TexCoords - Texture coordinates
@varying vec3 v_Normal - Normal vector in view space; normalize before use
@varying vec3 v_ViewDir - View vector from the fragment to the camera in view space; unnormalized, normalize before use
@varying vec4 v_Position - Fragment position in view space
@uniform vec3 u_Color - Base color of the fragment
@uniform float u_Opacity - Fragment opacity
@uniform float u_AlphaTest - Alpha cutoff below which fragments are discarded (USE_ALPHA_TEST)
@uniform mat4 u_Projection - Projection transformation matrix (ub_Camera)
@uniform mat4 u_View - View transformation matrix (ub_Camera)

*/

layout (location = 0) out vec4 o_FragColor;

#ifdef USE_INSTANCING
    in vec3 v_InstanceColor;
#endif

#ifdef USE_VERTEX_COLOR
    in vec3 v_Color;
#endif

in float v_ViewDepth;
in vec2 v_TexCoords;
in vec3 v_Normal;
in vec3 v_ViewDir;
in vec4 v_Position;

uniform float u_Opacity;
uniform vec3 u_Color;

#ifdef USE_ALPHA_TEST
    uniform float u_AlphaTest;
#endif

layout(std140) uniform ub_Camera {
    mat4 u_Projection;
    mat4 u_View;
};
