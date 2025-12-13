# Importing Assets

Asset importing in VGLX is an offline step. Before your application runs source files are converted into engine-native formats that are ready to upload to the GPU. The runtime never parses image or mesh formats. It only loads data that has already been prepared.

This design keeps the engine simple and predictable. Parsing formats like OBJ or PNG at runtime adds cost, complexity, and ambiguity. By moving that work into a build step VGLX can focus on rendering and scene management rather than file decoding.

VGLX uses two custom runtime formats. Meshes are stored as `.msh` files and textures as `.tex` files. These formats are intentionally minimal. They contain exactly the data the renderer needs and nothing more. Vertex layouts are explicit. Texture data is laid out linearly. There is no hidden work when an asset is loaded.


> VGLX never loads OBJ, PNG, or JPG files at runtime.<br/>
> All assets must be converted before running the application.

This approach mirrors how larger engines handle assets. Source files are flexible and convenient for artists. Runtime formats are fast and stable for engines. The asset builder bridges that gap and ensures that every asset your application loads is already in a form the GPU can consume efficient

## Asset Builder