# Installation

This page covers how to build VGLX, install it, and use it in your own project. The goal is to keep the setup simple. All dependencies are included in the repository, and you do not need anything system-wide beyond a C++ compiler, CMake and an OpenGL driver.

## Requirements

VGLX builds with a C++23-capable toolchain, [CMake](https://cmake.org/) 3.20 or newer, and an OpenGL 4.1+ context. It is regularly tested on the major platforms:

<div class="system-list">

- ![Ubuntu](https://raw.githubusercontent.com/EgoistDeveloper/operating-system-logos/master/src/16x16/UBT.png) Ubuntu 24.04 (GCC 11.3.0)
- ![macOS](https://raw.githubusercontent.com/EgoistDeveloper/operating-system-logos/master/src/16x16/MAC.png) macOS 14 (Clang 15.0.0)
- ![Windows](https://raw.githubusercontent.com/EgoistDeveloper/operating-system-logos/master/src/16x16/WIN.png) Windows 10 and MSVC 19.44

</div>

#### Dependencies

VGLX vendors all of its dependencies directly inside the repository. Nothing is downloaded at build time, and no external package managers are required. The runtime pieces are:

| Dependency                                | Version | Location       | Description                                                                         |
| ----------------------------------------- | ------- | -------------- | ----------------------------------------------------------------------------------- |
| [Glad](https://glad.dav1d.de/)            | 0.1.36  | `vendor/glad`  | OpenGL function loader generated from [glad.dav1d.de](https://glad.dav1d.de/).      |
| [GLFW](https://glfw.org/)                 | 3.5.0   | `vendor/glfw`  | Cross-platform window/input/context management (with minor internal modifications). |
| [ImGui](https://github.com/ocornut/imgui) | 1.92.1  | `vendor/imgui` | **Optional** immediate-mode UI library for in-engine tools and examples.            |

Each dependency includes its license inside the `vendor/` directory.

## VGLX Installer

The easiest way to install VGLX is using the Python installer included in the repository. It guides you through the process and builds the engine using the right presets for your system.

```bash
# clone the repository
git clone https://github.com/shlomnissan/vglx.git
cd vglx

# run the installer
python3 -m tools.installer.main
```

The installer checks for a working version of CMake, detects your compiler and asks for an installation prefix.

If you encounter issues, see the [Getting Help](#getting-help) section below.

## Creating a New Project

The quickest way to get started is cloning [the starter template](https://github.com/shlomnissan/vglx-starter):

```bash
 git clone https://github.com/shlomnissan/vglx-starter.git
```

It includes a simple application wired to VGLX using CMake. You can build and run it immediately. If you prefer starting from scratch, you have two options:

##### 1. Using CMake

If your project uses CMake, the recommended way to integrate VGLX is through `find_package`:

```cmake
find_package(vglx REQUIRED)
target_link_libraries(MyApp PRIVATE vglx::vglx)
```

CMake automatically picks the correct build configuration (Debug or Release) based on your project settings.

On Windows you may need to copy the VGLX DLL next to your application. You can automate this with:

```cmake
if(WIN32)
  add_custom_command(TARGET MyApp POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    $<TARGET_FILE:vglx::vglx>
    $<TARGET_FILE_DIR:MyApp>
  )
endif()
```

##### 2. Compile Directly

You can also link VGLX manually if you prefer using your compiler without CMake. The exact command depends on your platform and compiler. A minimal Linux example using `g++` might look like:

```bash
g++ main.cpp -o MyApp -I/usr/local/include -L/usr/local/lib -lvglx
```

Adjust include paths and library paths to match your system and compiler.

## Build From Source

The project includes several presets that streamline the process:

- `dev-debug` – Debug build with everything enabled
- `dev-release` – Optimized build with examples and tools
- `install-debug` – Debug install target (MSVC)
- `install-release` – Release install target

```bash [bash]
# clone the repository
git clone https://github.com/shlomnissan/vglx.git
cd vglx

# optional but recommended
mkdir build
cd build

# configure with a preset
cmake .. --preset dev-debug --config Debug

# build the engine
cmake --build . --config Debug
```

#### Configuration Options

VGLX includes optional build components. You can enable or disable them using standard CMake flags:

| Option                | Description                              |
| --------------------- | ---------------------------------------- |
| `VGLX_BUILD_DOCS`     | Build Doxygen documentation.             |
| `VGLX_BUILD_EXAMPLES` | Build example applications.              |
| `VGLX_BUILD_IMGUI`    | Enable ImGui support for debug UI/tools. |
| `VGLX_BUILD_TESTS`    | Build unit tests.                        |

Release presets build a shared library by default. If you prefer a static build, use:

```cmake
-DBUILD_SHARED_LIBS=OFF
```

#### Verifying Build

If examples are enabled (default in debug presets), two executables will appear:
`example_launcher_direct` and `example_launcher_runtime`.
Both run the same sandbox environment and help confirm that everything is set up correctly.

#### Manual Installation

If you want full control over the installation process, you can use CMake directly:

```bash
git clone https://github.com/shlomnissan/vglx.git
cd vglx

mkdir release
cd release

cmake .. --preset install-release --config Release
cmake --build . --config Release
cmake --install .
```

On Windows you may need to install both Debug and Release configurations due to ABI differences.

## Getting Help

If you run into issues, please [open an issue on GitHub](https://github.com/shlomnissan/vglx/issues). If possible include:

```text
- Your OS and compiler version
- CMake command you ran
- Installer or compiler logs
```

If you discover a fix, you are encouraged to open a PR with updates to this document so the whole community benefits.
