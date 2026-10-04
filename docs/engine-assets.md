# Engine Assets
Engine assets are meshes, shaders, materials, textures etc. that are common over multiple projects and rather than rewriting or copying a lambert or a skybox shader they should just be able to be imported into any project.

On compile engine assets will be copied into the 'engine' folder of an application and can then be referenced directly or via functions such as: 
```C/C++
std::string GetEngineShaderPath(const std::string& shaderName); (Utils.hpp)
```

For shader includes you can also just reference 'engine' and the shader compiler will handle it:
```glsl
#type vertex
#version 460 core
#include engine/cameraData.glsl

layout(location = 0) in vec3 Position;
layout(location = 1) in vec3 Normal;
...
```

Default engine UBOs such as CameraData, FrameData, etc. are set to use set 0 by default if need this to be different or certain ones require different bindings just copy the UBO into your shader and modify it there.