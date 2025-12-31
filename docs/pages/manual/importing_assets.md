# Importing Assets

Asset importing in VGLX is an offline step. Before your application runs source files are converted into engine-native formats that are ready to upload to the GPU. The runtime never parses image or mesh formats. It only loads data that has already been prepared.

This design keeps the engine simple and predictable. Parsing formats like OBJ or PNG at runtime adds cost, complexity, and ambiguity. By moving that work into a build step VGLX can focus on rendering and scene management rather than file decoding.

VGLX uses two custom runtime formats. Textures are stored as `.tex` files and meshes are stored as `.msh` files. These formats are intentionally minimal. They contain exactly the data the renderer needs and nothing more. Vertex layouts are explicit. Texture data is laid out linearly. There is no hidden work when an asset is loaded.

> VGLX never loads OBJ, PNG, or JPG files at runtime. All assets must be converted to runtime formats before running the application.

This approach mirrors how integrated engines handle assets. Source files are flexible and convenient for artists. Runtime formats are fast and stable for engines. The asset builder tool bridges that gap and ensures that every asset your application loads is already in a form the GPU can consume efficiently.

## Asset Builder

The asset builder tool converts source assets into VGLX runtime formats that the engine can load directly. The interface is intentionally simple. A single input asset produces one or more runtime files depending on what is being imported.

The easiest way to get the asset builder is through the VGLX installer. During installation you are prompted to install it alongside the engine. If enabled the installation directory you selected will contain a `bin` folder with the `asset_builder` executable.

It is recommended to add this directory to your system’s `PATH` so the tool can be invoked from anywhere. On Linux and macOS this usually means updating your shell configuration. On Windows the directory can be added through the system environment variables.

The asset builder is included in the VGLX repository. When building the project from source you must enable the `VGLX_BUILD_ASSET_BUILDER` option to include it. This option is disabled by default in project presets. Once built the executable can be run from the build output directory or copied into your project’s toolchain.

#### Supported File Formats

The asset builder currently supports a small set of source formats. These files are treated as input only and are never loaded by the engine at runtime:

| Asset Type | Supported Formats |
| ---------- | ----------------- |
| Meshes     | OBJ               |
| Textures   | JPG, JPEG, PNG    |

As the pipeline evolves additional source formats will be added.

#### CLI Options

The asset builder exposes a small set of command-line options. Output paths and file names can usually be inferred from the source asset.

| Option           | Description                                                |
| ---------------- | ---------------------------------------------------------- |
| `-i`, `--input`  | Input file to convert (e.g. PNG or OBJ).                   |
| `-o`, `--output` | Output directory. If omitted the source directory is used. |
| `-h`, `--help`   | Show help and exit.                                        |

## Importing Textures

In this section we import a texture into VGLX. We start by converting a JPG file into a `.tex` asset, then load it at runtime and apply it to a simple primitive.

The image used in this example is a simple [wooden crate texture](/manual_crate_texture_low.jpg). To convert this image or any other into a `.tex` asset run the asset builder from the command line and pass the image file using the `-i` flag:

```bash
asset_builder -i manual_crate_texture_low.jpg
```
If the conversion succeeds the asset builder prints the output path.

By default the asset builder writes the resulting `.tex` file next to the source image. You can override the output location using the `-o` option. The generated `.tex` file can be loaded directly by the engine without further processing.

Textures are loaded at runtime using the [TextureLoader](/reference/loaders/texture_loader). The loader is accessible from any node through the shared context which becomes available when the node is attached to the scene graph via a lifetime hook.

Asset loading is typically performed asynchronously to avoid blocking the main thread. To do this we store a [LoadHandle](/reference/loaders/load_handle) as a member of the scene and poll it during updates until the asset becomes available.

The following example revisits the rotating cube scene and applies a texture to it:

```cpp
#include <vglx/vglx.h>

struct MyScene : public vglx::Scene {
    std::shared_ptr<vglx::UnlitMaterial> material {
        vglx::UnlitMaterial::Create()
    };

    TextureLoadHandle handle {};
    vglx::Mesh* mesh {nullptr};

    MyScene() {
        mesh = Add(vglx::Mesh::Create(
            vglx::BoxGeometry::Create(),
            material
        ));
    }

    auto OnAttached(vglx::SharedContextPointer context) -> void override {
        handle = context->texture_loader->LoadAsync(
            "crate_texture_low.tex"
        );

        context->camera->TranslateZ(2.5f);
    }

    auto OnUpdate([[maybe_unused]] float delta) -> void override {
        if (auto result = handle.TryTake()) {
            material->texture_map = result.value();
        }

        const auto rotation_speed = vglx::math::pi_over_2;
        mesh->RotateX(rotation_speed * delta);
        mesh->RotateY(rotation_speed * delta);
    }
};
```

The asynchronous load is initiated in [OnAttached](/reference/scene/node#function-on-attached-369186a1). Once loading completes ownership of the texture is transferred to the application by calling [TryTake](/reference/loaders/load_handle#function-try-take-8debb008) on the handle in [OnUpdate](/reference/scene/node#function-on-update-5de00e28). If the load fails the handle can be queried for errors. Otherwise failures are reported through the engine logger.

If you used the same source image and followed the steps your application should produce a result similar to the image below:

![Textured Cube](/manual_02.webp)

If your application prints a `file not found` error make sure the texture asset is located in a directory that the application can access at runtime. By default assets are loaded using paths relative to the executable. If your assets live elsewhere provide an explicit relative path when calling the loader.

## Importing Meshes

In this section we import a mesh from an OBJ file. Unlike textures, meshes typically reference additional data such as material definitions and multiple textures. In this example the OBJ references an accompanying MTL file which includes a color map, a normal map, and a specular map.

When importing a mesh the asset builder treats the OBJ as the root of the asset. Geometry, material definitions, and all referenced textures are processed together. Each referenced image is converted to a `.tex` file, while the mesh geometry, material metadata, and texture paths are stored in a single `.msh` file.

The mesh used in this example is a [human head scan](/manual_lps_head.zip) by Lee Perry-Smith. After downloading the archive, unzip it and navigate to the extracted directory. From there, run the asset builder on the OBJ file:

```bash
asset_builder -i lps_head.obj -o output
```

After the command completes the output directory will contain one `.msh` file and several `.tex` files corresponding to the referenced textures. When loading the mesh at runtime these files must remain co-located so the engine can resolve material references correctly.

Meshes are loaded at runtime using the [MeshLoader](/reference/loaders/mesh_loader). As with textures the loader is accessed through the shared context and is only available once a node is attached.

The following snippet defines a custom scene that loads the imported mesh, resolves its materials and textures, and renders it with basic lighting:

```cpp
#include <vglx/vglx.h>

struct MyScene : public vglx::Scene {
    MeshLoadHandle handle {};

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
            "lps_head.msh"
        );

        context->camera->TranslateZ(2.5f);
    }

    auto OnUpdate([[maybe_unused]] float delta) -> void override {
        if (auto result = handle.TryTake()) {
            Add(std::move(result.value()));
        }
    }
};
```

If you followed the steps above using the same source files your application should produce a result similar to the image below.

![Human Head Scan](/manual_03.webp)

Loading a `.msh` file at runtime produces a fully constructed renderable node. The mesh geometry is uploaded to the GPU lazily, material parameters are initialized from the imported metadata, and any referenced .tex files are loaded and bound to the appropriate material slots. No additional setup is required by the application.

## Pipeline Overview

VGLX focuses on the graphics layer. The asset builder and runtime formats exist to provide a clear and predictable path from source data to GPU-ready resources. They are designed to remove format parsing and asset ambiguity from the runtime without imposing a heavy asset management system.

Asset loading in VGLX is explicit and ownership is clear. Loaders construct GPU-backed resources and return them to the call-site where they are owned and managed by the application. VGLX does not introduce global caches, registries, or lifetime management beyond this handoff. You load an asset, attach it, and use it.

This model keeps the engine small and composable. Applications that require caching, streaming, or higher-level asset systems are free to build those layers on top, while applications that do not need them are not forced to pay for their complexity.