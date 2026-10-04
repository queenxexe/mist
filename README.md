# Mist
Mist is a 3D game engine that utilizes Vulkan and SDL3 and can be compiled on Linux and Windows.

## Setup
To compile the project you will require [vcpkg](https://learn.microsoft.com/en-us/vcpkg/get_started/overview).

To build you should be able to compile via the clang/clang++ compiler as defined in the CMakePresets.json.

If unsure on IDE I personally develop using VSCode with the C/C++ extensions.

## Notes
- I don't upload most art assets that I use for testing so would require to be switched out with another file, could be .fbx or .obj but is a simple process.
- I do include shaders though as I can actually hold those on github and is good for all.
- While it was tested to compile on Windows previously. I don't actively use Windows to develop the engine so it's possible for issues on Windows but I will fix them if possible if I discover problems.

## Docs
There is LIMITED [Documentation](docs/getting-started.md) that is subject to change and is more just for me to help track changes and state quirks of the engine without having to look through the entire code base.

## Editor
I do work a little bit on the editor which I hope to one day be useful as a tool for making games but majority of my focus is on the engine and V.

## V
V I intend to be a voxel game but currently is used more as a test bench currently for engine features, but I do intend to develop it further and at least get it into a prototype state hopefully.