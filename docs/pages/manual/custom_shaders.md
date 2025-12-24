# Custom Shaders

In VGLX, shaders are attached to materials and materials are paired with geometry. A [Mesh](/reference/scene/mesh) is the fundamental renderable unit in the engine: it combines geometry, which describes what is drawn, with a material, which describes how that geometry is shaded.

The engine provides several built-in materials each backed by its own shader program. These cover common use cases such as unlit rendering and basic lighting. These materials are typically sufficient and allow you to build scenes without writing any shader code at all.

When you need more control over how geometry is transformed and shaded you use the [ShaderMaterial](http://localhost:5173/reference/materials/shader_material). This material allows you to supply your own vertex and fragment shader code written in GLSL. This gives you full control over how vertices are transformed and how fragments are colored, while still integrating cleanly with the rest of the engine.

VGLX uses [GLSL](https://en.wikipedia.org/wiki/OpenGL_Shading_Language) for all shader programs. Writing custom shaders assumes familiarity with shader programming concepts such as vertex and fragment stages, attributes, uniforms, and varyings. This guide does not teach GLSL. Instead, it focuses on how custom shaders fit in: how they are constructed, how they interact with engine-provided data, and how application-defined uniforms are passed to them.

In this guide we will write a minimal custom shader program, attach it to a shader material, and use it for rendering. Along the way we will look at how VGLX injects built-in attributes and varyings, and how custom uniforms are defined and updated from the application.

You can do a lot with VGLX without writing custom shaders but if you want full flexibility over how your geometry is rendered, custom shaders are the extension point, and this guide is where to start.