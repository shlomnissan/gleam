# Importing Assets

Asset importing in VGLX is an offline step. Before your application runs, source files are converted into engine-native formats that are ready to upload to the GPU. The runtime never parses image or mesh formats. It only loads data that has already been prepared.

This design keeps the engine simple and predictable. Parsing formats like OBJ or PNG at runtime adds cost, complexity, and ambiguity. By moving that work into a build step VGLX can focus on rendering and scene management rather than file decoding.

VGLX uses two custom runtime formats. Textures are stored as `.tex` files and meshes are stored as `.msh` files. These formats are intentionally minimal. They contain exactly the data the renderer needs and nothing more. Vertex layouts are explicit. Texture data is laid out linearly. There is no hidden work when an asset is loaded.


> VGLX never loads OBJ, PNG, or JPG files at runtime.<br/>
> All assets must be converted before running the application.

This approach mirrors how integrated engines handle assets. Source files are flexible and convenient for artists. Runtime formats are fast and stable for engines. The asset builder bridges that gap and ensures that every asset your application loads is already in a form the GPU can consume efficiently.

## Asset Builder

The asset builder CLI converts source assets into VGLX runtime formats that the engine can load directly. The interface is intentionally simple. A single input asset produces one or more runtime files depending on what is being imported.

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

| Option           | Description                                       |
| ---------------- | ------------------------------------------------- |
| `-i`, `--input`  | Input file to convert (e.g. PNG or OBJ).          |
| `-o`, `--output` | Output file path. If omitted a default is chosen. |
| `-h`, `--help`   | Show help and exit.                               |

## Importing Textures

In this section we import a texture into VGLX. We start by converting a JPG file into a `.tex` asset, then load it at runtime and apply it to a simple primitive.

The image used in this example is a simple [wooden crate texture](/crate_texture_low.jpg). To convert this image or any other into a `.tex` asset, run the asset builder from the command line and pass the image file using the `-i` flag:

```bash
asset_builder -i crate_texture_low.jpg
```
If the conversion succeeds the asset builder prints the output path.

By default the asset builder writes the resulting `.tex` file next to the source image. You can override the output location using the `-o` option. The generated `.tex` file can be loaded directly by the engine and uploaded to the GPU without further processing.

Textures are imported at runtime using the [TextureLoader](/reference/loaders/texture_loader). The loader is accessible from any node through the shared context which becomes available when the node is attached to the scene graph via the [OnAttached](/reference/scene/node#function-on-attached-ff71adbb) hook.

The following snippet defines a custom scene with a rotating cube.

```cpp
struct MyScene : public vglx::Scene {
    std::shared_ptr<vglx::UnlitMaterial> material =
        vglx::UnlitMaterial::Create();

    vglx::Mesh* mesh {nullptr};

    MyScene() {
        mesh = Add(vglx::Mesh::Create(
            vglx::BoxGeometry::Create(),
            material
        ));
    }

    auto OnAttached(vglx::SharedContextPointer context) -> void override {
        context->camera->TranslateZ(2.5f);

        // TODO: import texture...
    }

    auto OnUpdate(float delta) -> void override {
        const auto rotation_speed = vglx::math::pi_over_2;
        mesh->RotateX(rotation_speed * delta);
        mesh->RotateY(rotation_speed * delta);
    }
};
```

If this setup is unfamiliar refer back to [Creating an Application](/manual/creating_application).

To apply a texture to the cube, load the `.tex` asset inside the `OnAttached` hook. Once the texture is loaded, assign it to the mesh material:

```cpp
auto OnAttached(vglx::SharedContextPointer context) -> void override {
    context->camera->TranslateZ(2.5f);

    context->texture_loader->LoadAsync(
        "crate_texture_low.tex", [this](auto result) {
            // Always handle the case where result doesn't contain a value
            if (result) {
                material->texture_map = result.value();
            } else {
                std::println(std::cerr, "{}", result.error());
            }
        }
    );
}
```

If you used the same source image and followed the steps above, your application should produce a result similar to the image below:

![Window showing a textured cube](/importing_texture.png "Window showing a textured cube")

If your application prints a `file not found` error, make sure the texture asset is located in a directory that the application can access at runtime. By default assets are loaded using paths relative to the executable. If your assets live elsewhere, provide an explicit relative path when calling the loader.

## Importing Meshes

In this section we import a mesh from an OBJ file. Unlike textures, meshes typically reference additional data such as material definitions and multiple textures. In this example the OBJ references an accompanying MTL file, which defines a color map, a normal map, and a specular map.

When importing a mesh, `asset_builder` treats the OBJ as the root of the asset. Geometry, material definitions, and all referenced textures are processed together. Each referenced image is converted to a `.tex` file, while the mesh geometry, material metadata, and texture paths are stored in a single `.msh` file.

The mesh used in this example is a [human head scan](/lps_head.zip) by Lee Perry-Smith. After downloading the archive, unzip it and navigate to the extracted directory. From there, run the asset builder on the OBJ file:

```bash
asset_builder -i lps_head.obj -o output
```

After the command completes, the output directory will contain one `.msh` file and several `.tex` files corresponding to the referenced textures. When loading the mesh at runtime, these files must remain co-located so the engine can resolve material references correctly.

Meshes are loaded at runtime using the [MeshLoader](/reference/loaders/mesh_loader). As with textures, the loader is accessed through the shared context and is only available once a node is attached.

The following snippet defines a custom scene that loads the imported mesh, resolves its materials and textures, and renders it with basic lighting:

```cpp
struct MyScene : public vglx::Scene {
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
        context->camera->TranslateZ(2.5f);

        context->mesh_loader->LoadAsync(
            "lps_head.msh", [this](auto result){
                if (result) {
                    Add(std::move(result.value()));
                } else {
                    std::println(std::cerr, "{}", result.error());
                }
            }
        );
    }
};
```

If you followed the steps above using the same source files, your application should produce a result similar to the image below.

![Window showing a human head scan](/importing_mesh.png "Window showing a human head scan")

Loading a `.msh` file at runtime produces a fully constructed renderable node. The mesh geometry is uploaded to the GPU lazily, material parameters are initialized from the imported metadata, and any referenced .tex files are loaded and bound to the appropriate material slots. No additional setup is required by the application.

## Pipeline Overview

VGLX focuses on the graphics layer. The asset builder and runtime formats exist to provide a clear and predictable path from source data to GPU-ready resources. They are designed to remove format parsing and asset ambiguity from the runtime without imposing a heavy asset management system.

Asset loading in VGLX is explicit and ownership is clear. Loaders construct GPU-backed resources and return them to the call-site where they are owned and managed by the application. VGLX does not introduce global caches, registries, or lifetime management beyond this handoff. You load an asset, attach it, and use it.

This model keeps the engine small and composable. Applications that require caching, streaming, or higher-level asset systems are free to build those layers on top, while applications that do not need them are not forced to pay for their complexity.