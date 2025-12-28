# API Reference <Badge type="info">version 0.0.1</Badge> {.badged-header}

This section documents the public API of VGLX. It is generated directly from source and reflects the current state of the engine.

The reference is not a tutorial. It does not explain workflows or provide step-by-step examples. For that, start with the [guides in the manual](/manual/). Use the reference when you need exact information: types, functions, parameters, and guarantees.

The API is organized by responsibility. Core types define the application runtime. Scene and node types describe the scene graph. Materials, geometries, and data managers define how data reaches the GPU. Each page documents a single type and its public interface.

Most objects are created through factory functions. These return pointers that make ownership explicit. Once an object is returned its lifetime is owned by your code or by the scene graph it is attached to.

Use the reference as a lookup tool. Search for a type, follow related symbols, and read the documentation attached to each declaration.

If you find inconsistencies or missing details, contributions are welcome. Improving comments and documentation in the source directly improves the reference.