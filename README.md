## Geodesuka Engine v0.1.5

[![MIT licensed](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE.md)
[![forthebadge](https://forthebadge.com/images/badges/works-on-my-machine.svg)](https://forthebadge.com)

## State of the Engine

Branches/Versions 0.1.0, and 0.1.1 have been abandoned due to their uncompilable nature. Instead, modifications
will be slowly copied and pasted into this branch because of this. Version 0.1.2 is a branch of 0.0.21, which
was the first successful rendering of the engine.

# Description

The name "Geodesuka" has it's origins from a General Relativity class I took with a friend in 2017. It was
an ambitious project of building a real time relativistic renderer for the classes final project. We wanted
to utilize GPU computing for ray marching in curved space time to visually capture what an observer would
see in curved space time. This involved calculating light paths (null geodesics) with a particular metric
that described the curvature of a particular space and time. So we called the project a "Geodesic Renderer",
but then I had taken the liberty to weebify it and every once and a while change the window title to 
"Geodesuka Renderer". Needless to say, he was not impressed with the change. That is where the name "Geodesuka"
comes from, a weebified variant of the word "Geodesic", which is generally defined to be the shortest path within
a space.

After my first programming class I had taken an interest in GPU computing, not neccessarily for AI purposes, but
tasks outside of just rasterization and fixed pipeline rendering. What started out as a hobby project to more 
efficiently do expensive numerical computations such as fluid sims, vector field math, and large particles systems
is now being developed as a generalized API for GPGPU computation api for a game engine.

That is the final goal of this engine is to fully utilized general gpu usage into the backend of a game engine 
that can be used for purposes beyond just simple raster graphics or even hardware accelerated ray tracing. Perhaps
still an ambitious goal for a single person, but it is still a fun project to work on during the weekends.

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

# Third Party Libraries

This engine uses the following third party libraries.

Vulkan - Used for graphics.

OpenCL - https://software.intel.com/content/www/us/en/develop/tools/opencl-sdk.html
    The OpenCL SDK Developed by intel is used in this project. Must be installed before usage.
    If you wish to use another OpenCL SDK, just change the proper directories associated with
    the project files to do so. Otherwise go to the link provided to download Intel's OpenCL.

OpenXR - 
    N/A

GLFW - https://github.com/glfw/glfw
    GLFW is an Open Source, multi-platform library for OpenGL, OpenGL ES and Vulkan application development.
    It provides a simple, platform-independent API for creating windows, contexts and surfaces, reading input, 
    handling events, etc.

portaudio - http://portaudio.com/
    N/A

glslang: - https://github.com/KhronosGroup/glslang
    Is backend compiler to SPIRV in this project.



Assimp - https://assimp-docs.readthedocs.io/en/v5.1.0/
    N/A

FreeType - https://freetype.org/
    N/A

FreeImage - https://freeimage.sourceforge.io/
    N/A

# Reference Specifications:

Vulkan:
https://www.khronos.org/registry/vulkan/specs/1.2-extensions/html/vkspec.html

OpenCL:
https://www.khronos.org/registry/OpenCL/specs/3.0-unified/html/OpenCL_API.html#_the_opencl_architecture

