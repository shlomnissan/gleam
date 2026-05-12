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

The easiest way to install VGLX is with the Python installer included in the repository. It guides the process and builds the engine using the correct presets for your system.

```bash
# clone the repository
git clone https://github.com/shlomnissan/vglx.git
cd vglx

# run the installer
python3 -m tools.installer.main
```

The installer checks for CMake, detects your compiler, and asks for an installation prefix.

## Hello VGLX (minimal example)

```cpp
#include <vglx/vglx.hpp>

#include <print>

using namespace vglx;

auto main() -> int {
    auto window = Window {{
        .title = "Hello VGLX",
        .width = 1280,
        .height = 720
    }};

    if (auto result = window.Initialize(); !result.has_value()) {
        std::println(stderr, "{}", result.error());
        return 1;
    }

    auto renderer = Renderer {{
        .framebuffer_width = window.FramebufferWidth(),
        .framebuffer_height = window.FramebufferHeight()
    }};

    if (auto result = renderer.Initialize(); !result.has_value()) {
        std::println(stderr, "{}", result.error());
        return 1;
    }

    auto camera = PerspectiveCamera::Create({
        .fov = math::DegToRad(60.0f),
        .aspect = window.AspectRatio(),
        .near = 0.1f,
        .far = 1000.0f
    });

    window.OnResize([&camera, &renderer](const ResizeParameters& p) {
        renderer.SetViewport(0, 0, p.framebuffer_width, p.framebuffer_height);
        camera->Resize(p.window_width, p.window_height);
    });

    auto scene = Scene::Create();

    scene->Add(OrbitControls::Create(camera.get(), {
        .radius = 3.0f,
    }));

    scene->Add(Mesh::Create(
        BoxGeometry::Create(),
        PhongMaterial::Create(0x049EF4)
    ));

    scene->Add(PointLight::Create({
        .color = 0xFFFFFF,
        .intensity = 1.0f
    }))->transform.Translate({2.0f, 2.5f, 4.0f});

    auto timer = FrameTimer {true};
    while (!window.ShouldClose()) {
        window.PollEvents();
        scene->Advance(timer.Tick());
        renderer.Render(scene.get(), camera.get());
        window.SwapBuffers();
    }

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
