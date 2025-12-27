<h1 align="center">
   <b>
        <img style="margin: 1rem 0; width: 300px" src="https://github.com/user-attachments/assets/11593446-7123-46c1-8d10-4484b2be3513" /> 
    </b>
</h1>

<p align="center">A fast cross-platform 3D rendering engine for modern C++.

<div align="center">

![ubuntu-badge](https://github.com/shlomnissan/vglx/actions/workflows/ubuntu.yml/badge.svg)
![windows-badge](https://github.com/shlomnissan/vglx/actions/workflows/windows.yml/badge.svg)
![macos-badge](https://github.com/shlomnissan/vglx/actions/workflows/macos.yml/badge.svg)
[![docs-badge](https://img.shields.io/badge/docs-online-blue.svg)](https://shlomnissan.github.io/vglx/)

</div>

## Overview

VGLX is a scene-oriented rendering engine for modern C++ focused on immediacy and control. It provides a familiar scene-graph workflow built around objects, meshes, cameras, and lights, with native performance and explicit access to the GPU. The engine is fully cross-platform, running on Windows, macOS, and Linux.

#### Documentation

- Manual: https://vglx.org/manual/
- API reference: https://vglx.org/reference/
- Starter template: https://github.com/shlomnissan/vglx-starter

## Community driven

VGLX is developed in the open. All design decisions, trade-offs, and changes happen in public, in the repository. The goal is not to chase trends or grow fast but to build a rendering engine that stays understandable as it evolves. Simplicity, clarity, and mechanical sympathy matter more than feature count.

Community feedback shapes the engine. Questions, critiques, and experiments often lead to improvements in both the code and the documentation. If something feels unclear or unnecessarily complex, that signal is valuable. VGLX grows through shared understanding, not hidden roadmaps.

## Installation

The easiest way to install VGLX is using the Python installer included in the repository. It guides you through the process, builds the engine using the right presets for your system and can optionally install the asset builder CLI.


```bash
# clone the repository
git clone https://github.com/shlomnissan/vglx.git
cd vglx

# run the installer
python3 -m tools.installer.main
```

The installer checks for a working version of CMake, detects your compiler and asks for an installation prefix. If you plan to import optimized textures or models, enable the asset builder installation when prompted. The asset builder converts textures and meshes into GPU-friendly formats used by the engine. See Importing Assets to learn more.

## Contributions

Contributions are welcome at every level. Code, documentation, examples, bug reports, and design discussions all move the project forward. You do not need to arrive with a large feature in mind. Small, focused improvements are often the most impactful.

## License
```         
 __ __   ____  _      __ __ 
|  |  | /    || |    |  |  |
|  |  ||   __|| |    |  |  |
|  |  ||  |  || |___ |_   _|
|  :  ||  |_ ||     ||     |
 \   / |     ||     ||  |  |
  \_/  |___,_||_____||__|__|
                                     
Copyright (c) 2024–present Shlomi Nissan

The MIT License (MIT)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
```
