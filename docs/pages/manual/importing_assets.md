# Importing Assets

Asset importing in VGLX is an offline step. Before your application runs source files are converted into engine-native formats that are ready to upload to the GPU. The runtime never parses image or mesh formats. It only loads data that has already been prepared.

This design keeps the engine simple and predictable. Parsing formats like OBJ or PNG at runtime adds cost, complexity, and ambiguity. By moving that work into a build step VGLX can focus on rendering and scene management rather than file decoding.

VGLX uses two custom runtime formats. Textures are stored as `.tex` files and meshes are stored as `.msh` files. These formats are intentionally minimal. They contain exactly the data the renderer needs and nothing more. Vertex layouts are explicit. Texture data is laid out linearly. There is no hidden work when an asset is loaded.


> VGLX never loads OBJ, PNG, or JPG files at runtime.<br/>
> All assets must be converted before running the application.

This approach mirrors how integrated engines handle assets. Source files are flexible and convenient for artists. Runtime formats are fast and stable for engines. The asset builder bridges that gap and ensures that every asset your application loads is already in a form the GPU can consume efficiently.

## Asset Builder

The asset builder converts source assets into VGLX runtime formats. It runs offline and produces files that the engine can load directly. The interface is intentionally simple. A single input asset produces one or more runtime files depending on what is being imported.

The easiest way to get the asset builder is through the VGLX installer. During installation you are prompted to install it alongside the engine. If enabled the binary is placed next to the other VGLX tools and is ready to use immediately.

The asset builder is also included in the repository. When building VGLX from source you must enable the `VGLX_BUILD_ASSET_BUILDER` option to include it. This option is disabled by default in the project presets. Once built the executable can be run from the build output directory or copied into your project’s toolchain.

#### Supported File Formats

The asset builder currently supports a small set of source formats. These files are treated as input only and are never loaded by the engine at runtime.

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
