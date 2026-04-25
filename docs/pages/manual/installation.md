# Installation

This page covers how to build VGLX, install it, and use it in your own project. VGLX uses [vcpkg](https://vcpkg.io) to manage its third-party dependencies, so beyond a C++ compiler, CMake, and an OpenGL driver, you only need a working vcpkg installation.
## Requirements

VGLX builds with a C++23-capable toolchain, [CMake](https://cmake.org/) 3.20 or newer, [vcpkg](https://vcpkg.io), and an OpenGL 4.1+ context. It is regularly tested on the major platforms:

<div class="system-list">

- ![Ubuntu](https://raw.githubusercontent.com/EgoistDeveloper/operating-system-logos/master/src/16x16/UBT.png) Ubuntu 24.04 (GCC 11.3.0)
- ![macOS](https://raw.githubusercontent.com/EgoistDeveloper/operating-system-logos/master/src/16x16/MAC.png) macOS 14 (Clang 15.0.0)
- ![Windows](https://raw.githubusercontent.com/EgoistDeveloper/operating-system-logos/master/src/16x16/WIN.png) Windows 10 and MSVC 19.44

</div>

#### Setting up vcpkg

If you do not already have vcpkg installed, set it up once and export its location:

```bash
git clone https://github.com/microsoft/vcpkg.git ~/vcpkg
~/vcpkg/bootstrap-vcpkg.sh   # use bootstrap-vcpkg.bat on Windows
export VCPKG_ROOT=$HOME/vcpkg
```

VGLX's [`vcpkg.json`](https://github.com/shlomnissan/vglx/blob/main/vcpkg.json) declares the runtime dependencies and vcpkg installs them automatically the first time you configure the project. The first run takes one to two minutes per platform; subsequent runs reuse vcpkg's binary cache.

#### Dependencies

| Dependency | Source | Description |
|------------|--------|-------------|
| [Glad](https://github.com/Dav1dde/glad)   | vcpkg (`glad[gl-api-41]`) | OpenGL function loader for the 4.1 core profile. |
| [GLFW](https://glfw.org/)   | vcpkg (`glfw3`)    | Cross-platform window/input/context management. |
| [ImGui](https://github.com/ocornut/imgui)  | vcpkg (`imgui[glfw-binding,opengl3-binding]`)   | **Optional** immediate-mode UI library. Enabled by `VGLX_BUILD_IMGUI`. |
| [stb_image](https://github.com/nothings/stb)   | bundled (`vendor/`)  | Image loader for the texture pipeline. |
| [tinyobjloader](https://github.com/tinyobjloader/tinyobjloader)   | bundled (`vendor/`)  | OBJ mesh loader. |

The two `stb`-style headers are kept in `vendor/` because they are header-only and used purely as implementation details of the loader code.
## VGLX Installer

The easiest way to install VGLX is using the Python installer included in the repository. It guides you through the process and builds the engine using the right presets for your system.

```bash
# clone the repository
git clone https://github.com/shlomnissan/vglx.git
cd vglx

# run the installer (VCPKG_ROOT must be set)
python3 -m tools.installer.main
```

The installer checks for working versions of CMake and vcpkg, detects your compiler, and asks for an installation prefix.

::: tip
If you choose the default install prefix on Linux or macOS (`/usr/local`), you will need elevated privileges. Use `sudo -E` so that `VCPKG_ROOT` is preserved across the privilege boundary:

```bash
sudo -E python3 -m tools.installer.main
```

Or pick a user-writable custom prefix (e.g. `~/local/vglx`) when prompted, and skip `sudo` entirely.
:::

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

::: warning Consuming VGLX requires vcpkg
Because VGLX no longer bundles its third-party dependencies, your project must also be able to resolve `glad`, `glfw3`, and (if VGLX was built with ImGui support) `imgui[glfw-binding,opengl3-binding]` at configure time. The simplest path is to use vcpkg in your own project too:

```json
// vcpkg.json
{
  "name": "my-app",
  "version": "0.0.1",
  "dependencies": [
    "glad",
    "glfw3",
    { "name": "imgui", "features": ["glfw-binding", "opengl3-binding"] }
  ]
}
```

The starter template above ships with this configuration ready to go.
:::

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
g++ main.cpp -o MyApp \
    -I/usr/local/include -L/usr/local/lib -lvglx \
    -I$VCPKG_ROOT/installed/x64-linux/include \
    -L$VCPKG_ROOT/installed/x64-linux/lib -lglfw -limgui
```

Adjust include paths, library paths, and the vcpkg triplet to match your system.
## Build From Source

The project includes several presets that streamline the process. All presets load the vcpkg toolchain via `VCPKG_ROOT`, so make sure the environment variable is set before configuring.

- `dev-debug` – Debug build with everything enabled
- `dev-release` – Optimized build with examples and tools
- `install-debug` – Debug install target (MSVC)
- `install-release` – Release install target

```bash [bash]
# clone the repository
git clone https://github.com/shlomnissan/vglx.git
cd vglx

# configure with a preset
cmake --preset dev-debug

# build the engine
cmake --build build/debug
```
#### Configuration Options

VGLX includes optional build components. You can enable or disable them using standard CMake flags:

| Option                     | Description                              |
| -------------------------- | ---------------------------------------- |
| `VGLX_BUILD_DOCS`          | Build Doxygen documentation.             |
| `VGLX_BUILD_EXAMPLES`      | Build example applications.              |
| `VGLX_BUILD_IMGUI`         | Enable ImGui support for debug UI/tools. |
| `VGLX_BUILD_TESTS`         | Build unit tests.                        |

When building with `VGLX_BUILD_IMGUI=ON`, you must also pass `-DVCPKG_MANIFEST_FEATURES=imgui` so vcpkg installs the ImGui dependency. The bundled presets do this for you.

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

cmake --preset install-release
cmake --build out/install-release --config Release
cmake --install out/install-release --config Release
```

On Windows you may need to install both Debug and Release configurations due to ABI differences.

## Getting Help

If you run into issues, please [open an issue on GitHub](https://github.com/shlomnissan/vglx/issues). If possible include:

```text
- Your OS and compiler version
- vcpkg version (`vcpkg version`) and VCPKG_ROOT
- CMake command you ran
- Installer or compiler logs
```

If you discover a fix, you are encouraged to open a PR with updates to this document so the whole community benefits.
