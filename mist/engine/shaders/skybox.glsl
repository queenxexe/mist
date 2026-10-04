#cullmode off
#depthtest false
#type vertex
#version 460 core

layout(location = 0) out vec2 uv;

void main() {
    vec2 positions[3] = vec2[](
        vec2(-1.0, -1.0),
        vec2( 3.0, -1.0),
        vec2(-1.0,  3.0)
    );

    gl_Position = vec4(positions[gl_VertexIndex], 0.0, 1.0);
    uv = positions[gl_VertexIndex] * 0.5 + 0.5;
}

#type fragment
#version 460 core
#include engine/cameraData.glsl

layout(location = 0) in vec2 uv;
layout(location = 0) out vec4 color;

layout(set = 1, binding = 0) uniform sampler2D skybox;

vec2 directionToEquirectangularUV(vec3 dir) {
    dir = normalize(dir);
    float phi = atan(dir.z, dir.x);
    float theta = asin(dir.y);

    return vec2(
        0.5 + phi / (2.0 * 3.13159265),
        0.5 - theta / 3.13159265
    );
}

void main() {
    mat4 inverseVP = inverse(cameraData.ViewProjectionMatrix);
    vec2 ndc = uv * 2.0 - 1.0;

    vec4 nearPoint = inverseVP * vec4(ndc, 0.0, 1.0);
    nearPoint /= nearPoint.w;

    vec4 farPoint = inverseVP * vec4(ndc, 1.0, 1.0);
    farPoint /= farPoint.w;

    vec3 worldDir = normalize(farPoint.xyz - nearPoint.xyz);

    vec3 col = texture(skybox, directionToEquirectangularUV(worldDir)).rgb;
    color = vec4(col, 1.0);
}