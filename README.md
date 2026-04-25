<h1 align="center">
   <b>
        <img style="margin: 1rem 0; width: 300px" src="https://github.com/user-attachments/assets/11593446-7123-46c1-8d10-4484b2be3513" />
    </b>
</h1>

<p align="center">A cross-platform scene-oriented 3D rendering engine for modern C++</p>

<div align="center">

![windows-badge](https://github.com/shlomnissan/vglx/actions/workflows/windows.yml/badge.svg)
![macos-badge](https://github.com/shlomnissan/vglx/actions/workflows/macos.yml/badge.svg)
![ubuntu-badge](https://github.com/shlomnissan/vglx/actions/workflows/ubuntu.yml/badge.svg)
[![docs-badge](https://img.shields.io/badge/docs-online-blue.svg)](https://shlomnissan.github.io/vglx/)

</div>

## Overview

VGLX is a scene-oriented rendering engine for modern C++ focused on immediacy and control. It uses a familiar scene-graph built from objects, meshes, cameras, and lights with native performance and explicit GPU access. The engine is fully cross-platform and runs on Windows, macOS, and Linux.

#### Documentation

- Manual: https://vglx.org/manual/
- API reference: https://vglx.org/reference/
- Starter template: https://github.com/shlomnissan/vglx-starter

## Community driven

VGLX is developed in the open. Design decisions, trade-offs, and changes happen in public in the repository. The goal is not to chase trends or grow fast. It is to build a rendering engine that stays understandable as it evolves. Simplicity, clarity, and mechanical sympathy matter more than feature count.

Community feedback shapes the engine. Questions, critiques, and experiments often lead to improvements in both code and documentation. If something feels unclear or overly complex, that signal matters. VGLX grows through shared understanding, not hidden roadmaps.

## Installation

VGLX uses [vcpkg](https://vcpkg.io) to manage its third-party dependencies (`glad`, `glfw3`, and optionally `imgui`). Install vcpkg and export `VCPKG_ROOT` pointing to its location before running the installer:

```bash
git clone https://github.com/microsoft/vcpkg.git ~/vcpkg
~/vcpkg/bootstrap-vcpkg.sh   # use bootstrap-vcpkg.bat on Windows
export VCPKG_ROOT=$HOME/vcpkg
```

Then clone VGLX and run the Python installer. It checks for CMake and vcpkg, asks for an installation prefix, and builds the engine using the correct presets for your system.

```bash
git clone https://github.com/shlomnissan/vglx.git
cd vglx
python3 -m tools.installer.main
```

The first run will compile the vcpkg dependencies from source (one time, ~1–2 minutes); subsequent runs reuse vcpkg's binary cache.

> **Consuming VGLX in your project:** because VGLX no longer bundles ImGui, your project's CMake must also be able to resolve `imgui` (with the `glfw-binding` and `opengl3-binding` features) when VGLX was built with ImGui support. The simplest path is to use vcpkg in your project as well — see the starter template linked above for a working example.

## Hello VGLX (minimal example)

```cpp
#include <vglx/vglx.hpp>

using namespace vglx;

struct MyApp : public Application {
    auto Configure() -> Application::Parameters override {
        return { .title = "Hello VGLX" };
    }

    auto CreateScene() -> std::unique_ptr<Scene> override {
        GetContext()->camera->TranslateZ(3.0f);

        auto scene = Scene::Create();

        scene->Add(
            PointLight::Create({
                .color = 0xFFFFFF,
                .intensity = 1.0f
            })
        )->transform.Translate({2.0f, 2.5f, 4.0f});

        scene->Add(
            Mesh::Create(
                BoxGeometry::Create(),
                PhongMaterial::Create(0x049EF4)
            )
        )->RotateY(math::DegToRad(45.0f));

        return scene;
    }

    auto Update([[maybe_unused]] float dt) -> bool override {
        return true;
    }
};

auto main() -> int {
    auto app = MyApp {};
    app.Start();

    return 0;
}
```

## Contributions

Contributions are welcome at every level. Code, documentation, examples, bug reports, and design discussions all move the project forward. You do not need a large feature to contribute. Small, focused improvements often have the biggest impact.

## License
```
 ___      ___ ________  ___          ___    ___
|\  \    /  /|\   ____\|\  \        |\  \  /  /|
\ \  \  /  / | \  \___|\ \  \       \ \  \/  / /
 \ \  \/  / / \ \  \  __\ \  \       \ \    / /
  \ \    / /   \ \  \|\  \ \  \____   /     \/
   \ \__/ /     \ \_______\ \_______\/  /\   \
    \|__|/       \|_______|\|_______/__/ /\ __\
                                    |__|/ \|__|

The MIT License (MIT)

Copyright (c) 2025-present Shlomi Nissan
https://www.vglx.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
```
