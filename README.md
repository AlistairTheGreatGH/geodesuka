## Geodesuka Engine v0.1.5

[![MIT licensed](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE.md)
[![forthebadge](https://forthebadge.com/images/badges/works-on-my-machine.svg)](https://forthebadge.com)

## State of the Engine

Branches/Versions 0.1.0, and 0.1.1 have been abandoned due to their uncompilable nature. Instead, modifications
will be slowly copied and pasted into this branch because of this. Version 0.1.2 is a branch of 0.0.21, which
was the first successful rendering of the engine.

## Building & Compiling

# Windows

Requires Packages: VulkanSDK
cmake -S . -B bld/Windows/ -G "Visual Studio 17 2022"

# Linux

Requires Packages: gcc, git, cmake, xorg-dev, vulkan-sdk

Run installer shell file
./doc/install.sh
Then run the project builder
cmake -S . -B bld/Linux/

# Updated List:

- added thread_controller class, which will be used for interthread communication and timing.

- cleaned up string class.

- added FreeImage, FreeType, Assimp, 

# To Do List:

- Creating Error Logging System.

- Update variable.h to recognize vulkan types.

- Extract uniform variables from shader.

- math.h will be re worked to include fields.

- Add r1.h, r2.h, r3.h, r4.h, and vector field classes to engine.

- Add layering system for window objects, for huds, system stats and so
on. (Will be done with canvas class, and window as target.)

- Resolve issue with object being used in multiple stages simultaneously.

- Work on audio processing for game engine.

- Link asset libraries such as Assimp, FreeType, FreeImage, and so on to
geodesuka.

# Back Burner:

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

- Set up compilation unit directories to prevent source name space over writing.

- Add memory pool manager. 
