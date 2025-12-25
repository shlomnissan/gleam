# Custom Shaders

In VGLX, shaders are attached to materials and materials are paired with geometry. A [Mesh](/reference/scene/mesh) is the fundamental renderable unit in the engine: it combines geometry, which describes what is drawn, with a material, which describes how that geometry is shaded.

The engine provides several built-in materials each backed by its own shader program. These cover common use cases such as unlit rendering and basic lighting. These materials are typically sufficient and allow you to build scenes without writing any shader code at all.

When you need more control over how geometry is transformed and shaded you use the [ShaderMaterial](/reference/materials/shader_material). This material allows you to supply your own vertex and fragment shader code, giving you full control over vertex transformation and fragment shading while still integrating cleanly with the rest of the engine.

VGLX uses [GLSL](https://en.wikipedia.org/wiki/OpenGL_Shading_Language) for all shader programs. Writing custom shaders assumes familiarity with shader programming concepts such as vertex and fragment stages, attributes, uniforms, and varyings. This guide does not teach GLSL. Instead, it focuses on how custom shaders fit in: how they are constructed, how they interact with engine-provided data, and how application-defined uniforms are passed to them.

In this guide we will write a minimal custom shader program, attach it to a shader material, and use it for rendering. Along the way we will look at how VGLX injects built-in attributes and varyings, and how custom uniforms are defined and updated from the application.

You can do a lot with VGLX without writing custom shaders but if you want full flexibility over how your geometry is rendered custom shaders are the extension point and this guide is where to start.

## Shader Materials

A [ShaderMaterial](/reference/materials/shader_material) is a material backed by user-defined shaders. Unlike built-in materials, shader materials do not come with a predefined shader program. Instead, you provide code for both the vertex and fragment shaders when constructing the material.

VGLX keeps the structure around custom shaders intentionally light. Beyond a small set of built-in attributes, uniforms, and common varyings the engine does not impose a framework or shading model. This keeps shader materials flexible and makes it easy to integrate custom rendering techniques without fighting the engine.

To demonstrate how shader materials work we will start from the rotating cube scene introduced in the [previous guide](/manual/creating_application). This version uses a built-in unlit material that colors the cube with a single constant color:

```cpp
struct MyScene : public vglx::Scene {
    vglx::Mesh* mesh {nullptr};

    MyScene() {
        mesh = Add(vglx::Mesh::Create(
            vglx::BoxGeometry::Create(),
            vglx::UnlitMaterial::Create(0xFF00FF)
        ));
    }

    auto OnAttached(vglx::SharedContextPointer context) -> void override {
        context->camera->TranslateZ(2.5f);
    }

    auto OnUpdate(float delta) -> void override {
        const auto rotation_speed = vglx::math::pi_over_2;
        mesh->RotateX(rotation_speed * delta);
        mesh->RotateY(rotation_speed * delta);
    }
};
```

This scene renders a rotating cube in flat magenta. To illustrate how to use a custom shader we will replace the built-in material with a shader material that implements the same behavior: coloring all fragments with a single uniform color.

We begin by defining the vertex and fragment shader source code as raw string literals. These are placed near the top of the source file:

```cpp
constexpr auto vertex_shader = R"(
#version 410 core
#pragma inject_attributes

#include "snippets/vert_global_params.glsl"

void main() {
    #include "snippets/vert_main_varyings.glsl"

    gl_Position = u_Projection * u_ModelView * vec4(a_Position, 1.0);
}
)";

constexpr auto fragment_shader = R"(
#version 410 core
#pragma inject_attributes

#include "snippets/frag_global_params.glsl"

uniform vec3 color;

void main() {
    v_FragColor = vec4(color, 1.0);
}
)";
```

Before examining the shader code in detail let’s see how these shaders are used to construct a shader material and attach it to a mesh:

```cpp
vglx::Mesh* mesh {nullptr};
std::shared_ptr<vglx::ShaderMaterial> material;

MyScene() {
    material = vglx::ShaderMaterial::Create({
        .vertex_shader = vertex_shader,
        .fragment_shader = fragment_shader,
        .uniforms = {{"color", vglx::Color {0xFF00FF}}}
    });

    mesh = Add(vglx::Mesh::Create(
        vglx::BoxGeometry::Create(),
        material
    ));
}
```

Running the application now produces the same visual result as before. The difference is that the color is now applied by a custom shader program rather than a built-in material.

The [ShaderMaterial factory](/reference/materials/shader_material#function-create-54e37359) expects valid GLSL source code for both shader stages. Uniforms can be provided at construction time as a name/value map. These values are uploaded and bound automatically when the material is used for rendering.

If you have experience writing GLSL you may notice that the shader code contains a few elements that are specific to VGLX. Let’s break down the vertex shader first:

```glsl
#version 410 core
#pragma inject_attributes

#include "snippets/vert_global_params.glsl"

void main() {
    #include "snippets/vert_main_varyings.glsl"

    gl_Position = u_Projection * u_ModelView * vec4(a_Position, 1.0);
}
```

VGLX targets GLSL 4.10 core and every shader must begin with the corresponding version directive. Immediately after that is the `#pragma inject_attributes` directive. This is a VGLX-specific pragma that injects preprocessor definitions based on the material and program configuration. Like the version directive it should always appear at the top of the shader.

Shader code in VGLX is organized into reusable snippets. These snippets declare commonly used attributes, uniforms, and varyings. They are brought into the shader using standard `#include` directives. In the vertex shader we include `vert_global_params.glsl` to gain access to built-in vertex attributes and camera-related uniforms.

Inside `main` we include `vert_main_varyings.glsl` which defines varyings that are passed from the vertex stage to the fragment stage. Together, these snippets provide access to the vertex position attribute `a_Position` and the transformation matrices `u_Projection` and `u_ModelView`, which are required to transform vertices into clip space.

The fragment shader follows the same structure:

```glsl
#version 410 core
#pragma inject_attributes

#include "snippets/frag_global_params.glsl"

uniform vec3 color;

void main() {
    v_FragColor = vec4(color, 1.0);
}
```

Like the vertex shader, the fragment shader begins by injecting attributes and including global parameters. In this case the fragment-specific global parameters snippet is included. In addition, the fragment shader defines a custom uniform named `color`. This uniform is not populated by the engine automatically, it is provided by the application when the shader material is created:

```cpp
material = vglx::ShaderMaterial::Create({
    .vertex_shader = vertex_shader,
    .fragment_shader = fragment_shader,
    .uniforms = {{"color", vglx::Color {0xFF00FF}}}
});
```

If you plan to update uniforms dynamically it is a good idea to store the shader material instance as a member. For example, changing the cube’s color after creation can be done by updating the uniform value:

```cpp
material->uniforms["color"] = vglx::Color {0xFF0000};
```

With this minimal example we implemented a complete custom shader program and integrated it into VGLX. While the shader itself is simple the same structure scales to complex techniques. Shaders are the foundation of everything rendered on screen and shader materials provide the entry point for extending VGLX beyond its built-in materials.

## Shader Interface

VGLX shader programs rely on a small set of built-in attributes, uniforms, and varyings. These symbols are injected into shader code using the `#pragma inject_attributes` directive and standard include snippets, and provide access to common data.

The following tables list the symbols that are available in shader snippets.

#### Vertex Attributes

Vertex attributes describe per-vertex data supplied by the geometry. These attributes available in the vertex shader stage.

| Name         | Type   | Defined In                | Description                    |
| ------------ | ------ | ------------------------- | ------------------------------ |
| `a_Position` | `vec3` | `vert_global_params.glsl` | Vertex position in local-space |

#### Uniforms

Built-in uniforms provide per-draw or per-frame state supplied by the engine. These uniforms are declared in shader snippets and are populated automatically.

| Name         | Type   | Defined In                | Description                    |
| ------------ | ------ | ------------------------- | ------------------------------ |
| `u_Model` | `mat4` | `vert_global_params.glsl` | World-space transform |

#### Varyings

Varyings are interpolated values produced by the vertex shader and consumed by the fragment shader. They are declared by including the appropriate varyings snippets.

| Name         | Type   | Defined In                | Description                    |
| ------------ | ------ | ------------------------- | ------------------------------ |
| `v_ViewDepth` | `float` | `vert_main_varyings.glsl` | View-space depth |