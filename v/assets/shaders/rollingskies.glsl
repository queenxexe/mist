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
#define CLOUD_HEIGHT 1000.0
#define CLOUD_START_FADE 2000.0
#define CLOUD_END_FADE 5000.0

layout(location = 0) in vec2 uv;

layout(location = 0) out vec4 color;

layout(set = 0, binding = 0) uniform CameraData {
	uniform mat4 ViewProjectionMatrix;
	uniform vec3 CameraPosition;
} cameraData;

layout(set = 1, binding = 0) uniform FrameData {
	uniform float Time;
	uniform float DeltaTime;
} frameData;

vec2 mod289(vec2 x) {
	return x - floor(x * (1.0 / 289.0)) * 289.0;
}

vec3 mod289(vec3 x) {
	return x - floor(x * (1.0 / 289.0)) * 289.0;
}

vec4 mod289(vec4 x) {
	return x - floor(x * (1.0 / 289.0)) * 289.0;
}

vec3 permute(vec3 x, uint seed) {
	return mod289(((x * 34.0) + seed) * x);
}

float simplex2d(uint seed, vec2 v) {
	const vec4 C = vec4(0.211324865405187,		// (3.0-sqrt(3.0))/6.0
						0.366025403784439,		// 0.5*(sqrt(3.0)-1.0)
						-0.577350269189626,		// -1.0 + 2.0 * C.x
						0.024390243902439);		// 1.0 / 41.0

	// First corner
	vec2 i = floor(v + dot(v, C.yy));
	vec2 x0 = v - i + dot(i, C.xx);

	// Other corners
	vec2 i1 = vec2(step(x0.y, x0.x), step(x0.x, x0.y));
	vec4 x12 = x0.xyxy + C.xxzz;
	x12.xy -= i1;

	// Permutations
	i = mod289(i);
	vec3 p = permute(permute(
		i.y + vec3(0.0, i1.y, 1.0), seed) +
		i.x + vec3(0.0, i1.x, 1.0), seed);

	vec3 m = max(0.5 - vec3(dot(x0,x0), dot(x12.xy,x12.xy), dot(x12.zw,x12.zw)), 0.0);
	m = m * m;
	m = m * m;

	// Gradients: 41 points uniformly over a line, mapped onto a diamond.
	// The ring size 17*17 = 289 is close to a multiple of 41 (41*7 = 287)

	vec3 x = 2.0 * fract(p * C.www) - 1.0;
	vec3 h = abs(x) - 0.5;
	vec3 ox = floor(x + 0.5);
	vec3 a0 = x - ox;

	// Normalise gradients implicitly by scaling m
	// Approximation of: m *= inversesqrt( a0*a0 + h*h );
	m *= 1.79284291400159 - 0.85373472095314 * ( a0 * a0 + h * h );

	// Compute final noise value at P
	vec3 g;
	g.x = a0.x * x0.x + h.x * x0.y;
	g.yz = a0.yz * x12.xz + h.yz * x12.yw;
	return 130.0 * dot(m, g);
}

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