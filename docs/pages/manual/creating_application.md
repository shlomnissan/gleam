# Creating an Application

If you followed the [installation guide](/manual/installation) and verified your setup you are ready to start building. This page walks you through writing a minimal VGLX application and assembling a simple world from a few core pieces. The example is small on purpose. It introduces the fundamentals without burying you in details.

By the end you’ll have a working application and a clear sense of how the engine fits together, enough to [start exploring](/reference/core/application) on your own.

## Creating a Project

VGLX works best with [CMake](https://cmake.org/). In this section we create a small project so it can build cleanly on all platforms. To keep things simple we use a flat directory with just two files:

```text
/hello-vglx
  ├── CMakeLists.txt
  └── main.cpp
```

`CMakeLists.txt` holds a small build configuration:

```cmake
cmake_minimum_required(VERSION 3.20)

project(hello-vglx)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(hello-vglx main.cpp)

find_package(vglx REQUIRED)

target_link_libraries(hello-vglx PRIVATE vglx::vglx)

if (WIN32)
    add_custom_command(TARGET hello-vglx POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:vglx::vglx>
        $<TARGET_FILE_DIR:hello-vglx>
    )
endif()
```

The file starts by declaring the minimum CMake version, the project name and the language standard. We then create an executable and give it a single source file.

If you followed the installation guide you can import VGLX with two commands. `find_package(vglx REQUIRED)` asks CMake to locate the library. If it is missing or incorrectly installed, configuration fails. `target_link_libraries` links our application to the VGLX binaries.

The Windows-only section copies the VGLX DLL next to the executable after the build step. CMake selects the correct binary automatically based on your build type. Without this the application
may fail to compile on Window unless you add the binaries to your system path.

This setup looks simple but CMake is doing a lot behind the scenes. It verifies the installation, loads the correct configuration and handles platform-specific details for us.

Before we move on, let’s add a minimal `main.cpp` to test that the project builds correctly:

```cpp
#include <print>
#include <vglx/vglx.hpp>

auto main() -> int {
    std::println("hello, world");
    return 0;
}
```

With both files in place you can configure and build the project from the project root:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --config Debug
```

After the build completes you should see the executable in the build directory.
- Linux/macOS: `build/hello-vglx`
- Windows: `build/Debug/hello-vglx.exe`

Run it and you should be greeted by your application.

If CMake reports that it cannot find VGLX return to the [installation guide](/manual/installation) and verify that the library was installed to a prefix CMake can locate. Otherwise you are ready to move on to the application entrypoint.

## Application Runtime

The preferred way to create a VGLX application is using the application runtime. The runtime sets up the window, the rendering context, the main loop, and calls your hooks. You create a runtime instance by subclassing the [Application](/reference/core/application) class.

We can add a runtime instance to our bare-bones `main.cpp` file:

```cpp
#include <vglx/vglx.hpp>

struct MyApp : public vglx::Application {
    auto Configure() ->  Application::Parameters override {
        return {
            .title = "Hello VGLX",
            .clear_color = {0x000000},
            .width = 1024,
            .height = 768,
            .antialiasing = 4,
            .vsync = false,
            .show_stats = true,
        };
    }

    auto CreateScene() -> std::shared_ptr<vglx::Scene> override {
        return vglx::Scene::Create();
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

Our class overrides three functions that initialize the runtime. [Configure](/reference/core/application#function-configure-0192d125) is optional but you will often implement it. It returns a small data object that describes how the application should start up. Using designated initializers keeps each field clear and easy to read.

[CreateScene](/reference/core/application#function-create-scene-2b0beeb4) is required and returns the scene you want to render. At this stage we simply return an empty scene using its factory. VGLX uses shared pointers to store nodes in the scene graph and built-in nodes provide [Create](/reference/nodes/scene#function-create-a8164366) helpers to construct them correctly.

The last function, [Update](/reference/core/application#function-update-d40fe494), is also required. It is called once per frame and is where you add per-frame logic at the application level. Returning `true` keeps the application running. Returning `false` exits the main loop.

In `main` we create an instance of the app and call [Start](/reference/core/application#function-start-28ef28d7) to launch it. If you build and run the project again you should see a window for your new VGLX application.

## Your First Scene