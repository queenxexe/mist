# Shaders

## Notes
- Currently only vertex and fragment shaders have been tested to work but the compiler should in theory be able to handle others such as compute shaders.
- Currently only .glsl shaders are supported I may look at doing others in the future.

## The docs
Shaders are all held in a single file and the stages are denoted by '#type' followed by the stage name.
```glsl
#type vertex
#version 460 core
#include engine/cameraData.glsl

layout(location = 0) in vec3 Position;
layout(location = 1) in vec3 Normal;
...
```

## UBOs
There is some default engine assets that include basic UBOs that the engine can provide automatically and are set to use set 0. If you require them to use different sets/bindings for any reason you will have to simply copy the include in and modify it within your own shader.

For more info on the engine assets look here [Engine Asset Documentation](engine-assets.md)

## Culling
At the start of the shader you can define '#cullmode' followed by either 'back', 'front', or 'off'. Which will then use that as its culling setting in engine.

```glsl
#cullmode off
#type vertex
#version 460 core
#include engine/cameraData.glsl

layout(location = 0) in vec3 Position;
layout(location = 1) in vec3 Normal;
...
```

## Depth testing
Depth testing currently can only be turned on or off via defining '#depthtest' and setting it to 'true' or 'false'.

```glsl
#depthtesting false
#type vertex
#version 460 core
#include engine/cameraData.glsl

layout(location = 0) in vec3 Position;
layout(location = 1) in vec3 Normal;
...
```