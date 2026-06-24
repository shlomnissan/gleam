/*

This vertex shader snippet defines the input attributes, uniforms, and varyings
used in both the vertex and fragment shaders.

@in vec3 a_Position - Vertex position
@in vec3 a_Normal - Vertex normal
@in vec2 a_TexCoord - Vertex texture coordinate
@in mat4 a_InstanceTransform - Instance transformation matrix (USE_INSTANCING)
@in vec3 a_InstanceColor - Instance color (USE_INSTANCING)
@in vec3 a_Color - Vertex color (USE_VERTEX_COLOR)
@uniform mat3 u_TextureTransform - Applies texture coordinate transformations
@uniform mat4 u_Model - Model transformation matrix
@uniform mat4 u_Projection - Projection transformation matrix
@uniform mat4 u_View - View transformation matrix
@out float v_ViewDepth - Depth of the vertex in view space
@out vec2 v_TexCoords - Transformed texture coordinates for the fragment shader
@out vec3 v_Normal - Transformed normal vector in view space
@out vec3 v_ViewDir - View vector from the vertex to the camera in view space; unnormalized
@out vec4 v_Position - Vertex position in view space

*/

in vec3 a_Position;
in vec3 a_Normal;
in vec2 a_TexCoord;

#ifdef USE_INSTANCING
    in mat4 a_InstanceTransform;
    in vec3 a_InstanceColor;
    out vec3 v_InstanceColor;
#endif

#ifdef USE_VERTEX_COLOR
    in vec3 a_Color;
    out vec3 v_Color;
#endif

uniform mat3 u_TextureTransform;
uniform mat4 u_Model;

out float v_ViewDepth;
out vec2 v_TexCoords;
out vec3 v_Normal;
out vec3 v_ViewDir;
out vec4 v_Position;

layout(std140) uniform ub_Camera {
    mat4 u_Projection;
    mat4 u_View;
};
