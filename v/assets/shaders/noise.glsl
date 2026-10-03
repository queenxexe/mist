// Description : Array and textureless GLSL 2D simplex noise function.
//      Author : Ian McEwan, Ashima Arts.
//  Maintainer : stegu
//     Lastmod : 20110822 (ijm)
//     License : Copyright (C) 2011 Ashima Arts. All rights reserved.
//               Distributed under the MIT License. See LICENSE file.
//               https://github.com/ashima/webgl-noise
//               https://github.com/stegu/webgl-noise

// This shader has been adapted to have seeding 
// as well as adding my own helper functions

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

vec4 permute(vec4 x, uint seed) {
	return mod289(((x * 34.0) + seed) * x);
}

vec4 taylorInvSqrt(vec4 r) {
	return 1.79284291400159 - 0.85373472095314 * r; 
}

float simplex2d(uint seed, vec2 v) {
	const vec4 C = vec4(0.211324865405187,	// (3.0-sqrt(3.0))/6.0
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
	m *= 1.79284291400159 - 0.85373472095314 * ( a0*a0 + h*h );

	// Compute final noise value at P
	vec3 g;
	g.x = a0.x * x0.x + h.x * x0.y;
	g.yz = a0.yz * x12.xz + h.yz * x12.yw;
	return 130.0 * dot(m, g);
}

float simplex3d(uint seed, vec3 v) {
	const vec2 C = vec2(1.0 / 6.0, 1.0 / 3.0);

	// First corner
	vec3 i = floor(v + dot(v, C.yyy));
	vec3 x0 = v - i + dot(i, C.xxx);

	// Other corners
	vec3 g = step(x0.yzx, x0.xyz);
	vec3 l = 1.0 - g;
	vec3 i1 = min(g.xyz, l.zxy);
	vec3 i2 = max(g.xyz, l.zxy);

	vec3 x1 = x0 - i1 + C.xxx;
	vec3 x2 = x0 - i2 + C.yyy;
	vec3 x3 = x0 - 0.5;

	// Permutations
	i = mod289(i);
	vec4 p = permute(permute(permute(
		i.z + vec4(0.0, i1.z, i2.z, 1.0), seed) +
		i.y + vec4(0.0, i1.y, i2.y, 1.0), seed) +
		i.x + vec4(0.0, i1.x, i2.x, 1.0), seed);

	// Gradients: 7x7 points over a square, mapped onto an octahedron.
	// The ring size 17*17 = 289 is close to a multiple of 49 (49*6 = 294)
	vec4 j = p - 49.0 * floor(p / 49.0);

	vec4 x_ = floor(j / 7.0);
	vec4 y_ = floor(j - 7.0 * x_);

	vec4 x = (x_ * 2.0 + 0.5) / 7.0 - 1.0;
	vec4 y = (y_ * 2.0 + 0.5) / 7.0 - 1.0;

	vec4 h = 1.0 - abs(x) - abs(y);

	vec4 b0 = vec4(x.xy, y.xy);
	vec4 b1 = vec4(x.zw, y.zw);

	vec4 s0 = floor(b0) * 2.0 + 1.0;
	vec4 s1 = floor(b1) * 2.0 + 1.0;
	vec4 sh = -step(h, vec4(0.0));

	vec4 a0 = b0.xzyw + s0.xzyw * sh.xxyy;
	vec4 a1 = b1.xzyw + s1.xzyw * sh.zzww;

	vec3 g0 = vec3(a0.xy, h.x);
	vec3 g1 = vec3(a0.zw, h.y);
	vec3 g2 = vec3(a1.xy, h.z);
	vec3 g3 = vec3(a1.zw, h.w);

	// Normalise gradients
	vec4 norm = taylorInvSqrt(vec4(dot(g0, g0), dot(g1, g1), dot(g2, g2), dot(g3, g3)));
	g0 *= norm.x;
	g1 *= norm.y;
	g2 *= norm.z;
	g3 *= norm.w;

	// Mix final noise value
	vec4 m = max(0.6 - vec4(dot(x0, x0), dot(x1, x1), dot(x2, x2), dot(x3, x3)), 0.0);
	m = m * m;
	m = m * m;

	vec4 px = vec4(dot(x0, g0), dot(x1, g1), dot(x2, g2), dot(x3, g3));
	return 42.0 * dot(m, px);
}

float hash(uint seed, vec3 p) {
	vec3 seedOffset = vec3(
		seed * 0.1031,
		seed * 0.11369,
		seed * 0.13787
	);
	p += seedOffset;

	return fract(sin(dot(p, vec3(127.1, 311.7, 74.7))) * 43758.5453);
}

vec3 randomPoint(uint seed, vec3 cell, vec3 jitter) {
	return cell + vec3(
		hash(seed, cell),
		hash(seed, cell + 31.34),
		hash(seed, cell + 91.17)
	) * jitter;
}

// Worley Voronoi
float voronoi(uint seed, vec3 p) {
	vec3 cell = floor(p);
	vec3 jitter = vec3(simplex3d(seed, p));
	float minDist = 99999.0;

	for (int x = -1; x <= 1; ++x) {
		for (int y = -1; y <= 1; ++y) {
			for (int z = -1; z <= 1; ++z) {
				vec3 neighbour = cell + vec3(x,y,z);
				vec3 feature = randomPoint(seed, neighbour, jitter);
				float d = distance(p, feature);
				minDist = min(minDist, d);
			}
		}
	}

	return minDist;
}

float hash(uint seed, vec2 p) {
	vec3 p3 = fract(vec3(p, seed) * vec3(0.1031, 0.11369, 0.13782));
	p3 += dot(p3, p3.yzx + 19.19);
	return fract((p3.x + p3.y) * p3.z);
}

vec2 hash2(uint seed, vec2 p) {
	return vec2(
		hash(seed, p),
		hash(seed + 1u, p)
	);
}

float voronoi(uint seed, vec2 p) {
	vec2 cell = floor(p);
	vec2 fractional = p - cell;
	float minDist = 8.0;
	for (int x = -1; x <= 1; x++) {
		for (int y = -1; y <= 1; y++) {
			vec2 neighbor = cell + vec2(x, y);
			vec2 pos = neighbor + hash2(seed, neighbor);
			float d = distance(fractional, pos - cell);
			minDist = min(minDist, d);
		}
	}
	return minDist;
}