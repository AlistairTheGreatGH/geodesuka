## Geodesuka Engine v0.1.5

[![MIT licensed](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE.md)
[![forthebadge](https://forthebadge.com/images/badges/works-on-my-machine.svg)](https://forthebadge.com)

## State of the Engine

Currently working on the 3d rendering for the engine. Still in conceptual phase, determining material system.

## Building & Compiling
The geodesuka engine has been designed to make it as easy as possible to integrate
with your project provided you have the installed dependencies. The geodesuka uses
CMake to generate project build files to maintain a cross platform build system. 
Provided you know how to use CMake, it should be fairly easy to configure your project
to use the geodesuka engine. The first depedency, which you will need to use the geodesuka
engine is CMake. CMake is used to generate project build files so you can compile the source
for the targeted platform. Secondly, which is irrelevant to what ever platform you wish to 
compile from is the Vulkan SDK. To actually interact with GPU hardware you need an API to 
do so, and Vulkan is that API. After 

CMake:
https://cmake.org/

Vulkan SDK:
https://www.lunarg.com/vulkan-sdk/

# Windows
On Windows, you can use MSVC, MinGW, or whatever compiler set is most convenient to you.
Most of the development of this engine is done on Windows using MSVC++. 

Visual Studio IDE:
https://visualstudio.microsoft.com/vs/


cmake -S . -B bld/ -G "Visual Studio 17 2022"

# Linux
On Linux, you will probably end up using GCC, or CLang, or whatever (IDGAF). You
just need to insure that you have those compilers installed, you need a windowing 
server system to interact with. Either X11 or Wayland, up to you. I don't care. Or
you can use the install.sh script in the doc/ folder to auto install the dependencies.

cmake -S . -B bld/ -G "Unix Makefiles"

# To Do List:

- Finish model.h material system.

- Clean up pipeline.h to generate descriptor sets for model.h

- Resolve issue with object being used in multiple stages simultaneously.

# Back Burner:

- Introduce portaudio to the geodesuka engine.

- Add layering system for window objects, for huds, system stats and so
on. (Will be done with canvas class, and window as target.)

- File System stuff.

- Add built in extension types for file.h to recognize file types
and forward to proper objects.

- Add lua support for runtime scripting.

- Add engine asset manager to prevent double loading.

- Add Dynamic Library compilation options.

- Change Texture class to image class? The reasoning behind this change
along with how vulkan does it, is that a texture describes the texture
of a particular surface while an image is a generalized concept of a type
of memory.
