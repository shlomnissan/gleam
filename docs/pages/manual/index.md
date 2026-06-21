![Scene by Karthik Naidu rendered in VGLX](/hero.webp "Scene by Karthik Naidu rendered in VGLX")

## Introduction

[VGLX](https://github.com/shlomnissan/vglx) is a scene-oriented rendering engine for modern C++ with a simple philosophy: building things in 3D should feel good again. The engine aims to bring back the sense of immediacy that early rendering APIs once had while still giving full access to the power of today’s GPUs. It is fully cross-platform and runs on Windows, macOS and Linux with identical behavior.

The name originally stood for Cross-Platform Vulkan Graphics Library, but practicality won the early battles so the backend today is OpenGL. Vulkan support is planned for 2026. In the meantime OpenGL lets the engine grow without drowning in ceremony.

A rendering engine should help you build scenes, not fight them. Everything revolves around a familiar structure of objects, meshes, cameras and lights arranged in a scene graph. You compose worlds from simple pieces, apply transforms and let the renderer do the work. If you have used Three.js the workflow will feel familiar. The difference is that VGLX runs natively and gives you fine control over how things reach the GPU.

This engine focuses on rendering. It is not a full game engine. There are no prefabs, audio systems or physics. These parts are left for you to design. VGLX stays small on purpose. It gives you a solid foundation for real-time graphics without locking you into a specific architecture.

VGLX is still young, but the direction is stable. The engine already supports meshes, materials, textures and basic lighting. The reference section offers the most up-to-date view of what is available. The goal is simple. Keep the API clear. Keep it predictable. Make it a tool you can learn quickly and rely on when you want full control.

There is much more to build, but the foundation is here. If you want to explore real-time graphics with modern C++ and a clean design, [VGLX is ready for you](/manual/installation).

## Contributions

VGLX is built in the open and contributions are encouraged. The engine has plenty of room to grow, and developers who enjoy clean design and real-time graphics are welcome to join. Improvements to code, documentation and examples all help move the project forward. If you want to take on a missing feature, [open an issue on GitHub](https://github.com/shlomnissan/vglx) to start a discussion. The repository provides everything you need to begin exploring.
