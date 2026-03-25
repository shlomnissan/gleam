# Importing Assets

VGLX loads standard asset formats directly at runtime. There is no offline conversion step and no intermediate file formats. You point a loader at a PNG or an OBJ and the engine handles the rest.

This keeps the workflow simple. Assets go straight from your content tools into the engine without an extra build step in between. The loaders decode source files, construct GPU-ready resources, and hand them back to your application.

#### Supported Formats

| Asset Type | Supported Formats       |
| ---------- | ----------------------- |
| Textures   | PNG, JPEG, TGA, BMP, HDR |
| Meshes     | OBJ (glTF support coming soon)                    |

## Loading Textures

In this section we load a texture and apply it to a simple primitive. The image used in this example is a [wooden crate texture](/crate_texture_low.jpg). Any supported image format will work.

Textures are loaded at runtime using the [TextureLoader](/reference/loaders/texture_loader). The loader is accessible from any node through the shared context which becomes available when the node is attached to the scene graph via a lifetime hook.

Asset loading is typically performed asynchronously to avoid blocking the main thread. To do this we store a [LoadHandle](/reference/loaders/load_handle) as a member of the scene and poll it during updates until the asset becomes available.

The following example revisits the rotating cube scene from the [previous guide](/manual/creating_application):
```cpp
#include <vglx/vglx.h>

struct MyScene : public vglx::Scene {
    vglx::TextureLoadHandle handle {};
    vglx::Mesh* mesh {nullptr};

    auto OnAttached(vglx::SharedContextPointer context) -> void override {
        handle = context->texture_loader->LoadAsync(
            "crate_texture_low.jpg"
        );
        context->camera->TranslateZ(2.5f);
    }

    auto OnUpdate([[maybe_unused]] float delta) -> void override {
        if (auto result = handle.TryTake()) {
            auto geometry = vglx::BoxGeometry::Create();
            auto material = vglx::UnlitMaterial::Create(0xFFFFFF);

            material->texture_map = result.value();
            mesh = Add(vglx::Mesh::Create(geometry, material));
        }

        if (mesh != nullptr) {
            const auto rotation_speed = vglx::math::pi_over_2;
            mesh->RotateX(rotation_speed * delta);
            mesh->RotateY(rotation_speed * delta);
        }
    }
};
```

The asynchronous load is initiated in [OnAttached](/reference/scene/node#function-on-attached-369186a1). Once loading completes ownership of the texture is transferred to the application by calling [TryTake](/reference/loaders/load_handle#function-try-take-8debb008) on the handle in [OnUpdate](/reference/scene/node#function-on-update-5de00e28). If the load fails the handle can be queried for errors. Otherwise failures are reported through the engine logger.

If you used the same source image and followed the steps your application should produce a result similar to the image below:

![Textured Cube](/manual_02.webp)

If your application prints a `file not found` error make sure the image is located in a directory that the application can access at runtime. By default assets are loaded using paths relative to the executable. If your assets live elsewhere provide an explicit relative path when calling the loader.

## Loading Meshes

In this section we load a mesh from an OBJ file. Unlike textures, meshes typically reference additional data such as materials and textures. In this example the OBJ references an accompanying MTL file which includes a color map, a normal map, and a specular map.

When loading a mesh the engine treats the OBJ as the root of the asset. Geometry, materials, and textures are resolved together. Referenced images are loaded from the same directory as the OBJ file and bound to the appropriate material slots automatically.

The mesh used in this example is a [human head scan](/lps_head.zip) by Lee Perry-Smith. Unzip the archive and place the contents alongside the executable to follow along.

Meshes are loaded at runtime using the [MeshLoader](/reference/loaders/mesh_loader). As with textures the loader is accessed through the shared context and is only available once a node is attached.

The following example loads the mesh and renders it with basic lighting:

```cpp
#include <vglx/vglx.h>

struct MyScene : public vglx::Scene {
    vglx::MeshLoadHandle handle {};

    MyScene() {
        Add(vglx::AmbientLight::Create({
            .color = 0xFFFFFF,
            .intensity = 0.5f
        }));

        Add(vglx::PointLight::Create({
            .color = 0xFFFFFF,
            .intensity = 1.0f
        }))->transform.Translate({-2.0f, 2.5f, 4.0f});
    }

    auto OnAttached(vglx::SharedContextPointer context) -> void override {
        handle = context->mesh_loader->LoadAsync(
            "lps_head.obj"
        );

        context->camera->TranslateZ(3.5f);
    }

    auto OnUpdate([[maybe_unused]] float delta) -> void override {
        // Note that we use std::move(result.value())
        // to transfer ownership to the scene graph
        Add(std::move(result.value()))->RotateY(
            vglx::math::DegToRad(90.0f)
        );
    }
};
```

If you followed the steps above using the same source files your application should produce a result similar to the image below:

![Human Head Scan](/manual_03.webp)

Loading an OBJ file at runtime produces a fully constructed renderable node. The mesh geometry is uploaded to the GPU lazily, material parameters are initialized from the MTL data, and any referenced textures are loaded and bound to the appropriate material slots. No additional setup is required by the application.

## Overview

Asset loading in VGLX is explicit and ownership is clear. Loaders construct GPU-backed resources and return them to the call-site where they are owned and managed by the application. VGLX does not introduce global caches, registries, or lifetime management beyond this handoff. You load an asset, attach it, and use it.

The loader infrastructure is built around a simple class that can be extended to support custom file formats. See the [LoadHandle](/reference/loaders/load_handle) reference for details on implementing your own.

This model keeps the engine small and composable. Applications that require caching, streaming, or higher-level asset systems are free to build those layers on top, while applications that do not need them are not forced to pay for their complexity.
