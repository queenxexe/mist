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
#include engine/frameData.glsl
#include noise.glsl

#define CLOUD_HEIGHT 1000.0
#define CLOUD_START_FADE 2000.0
#define CLOUD_END_FADE 5000.0

layout(location = 0) in vec2 uv;

layout(location = 0) out vec4 color;

vec3 getWorldRay(vec2 uv) {
	mat4 inverseVP = inverse(cameraData.ViewProjectionMatrix);

	vec2 ndc = uv * 2.0 - 1.0;
	vec4 nearPoint = inverseVP * vec4(ndc, 0.0, 1.0);
	vec4 farPoint = inverseVP * vec4(ndc, 1.0, 1.0);
	
	nearPoint /= nearPoint.w;
	farPoint /= farPoint.w;

	return normalize(farPoint.xyz - nearPoint.xyz); 
}

vec3 renderSky(vec3 ray) {
	if (ray.y > 0.05) {
		float t = (CLOUD_HEIGHT - cameraData.CameraPosition.y) / ray.y;
		vec3 cloudPosition = cameraData.CameraPosition + ray * t;

		float large = simplex2d(1, cloudPosition.xz * 0.003 + vec2(0.05, 0.05) * frameData.Time);
		float detail = simplex2d(1, cloudPosition.xz * 0.001 + vec2(-0.1, 0.15) * frameData.Time);
		float n = large * 0.8 + detail * 0.2;

		float cloudShape = smoothstep(0.3, 0.6, n);
		float dist = length(cloudPosition.xz - cameraData.CameraPosition.xz);
		float cloudFade = 1.0 - smoothstep(CLOUD_START_FADE, CLOUD_END_FADE, dist);

		return vec3(cloudShape * cloudFade);
	} else {
		return vec3(0.0, 0.0, 0.0);
	}
}

void main() {
	vec3 ray = getWorldRay(uv);
	vec3 sky = renderSky(ray);
	color = vec4(sky, 1.0);
}