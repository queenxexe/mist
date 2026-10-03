// Description : Array and textureless GLSL 2D simplex noise function.
//      Author : Ian McEwan, Ashima Arts.
//  Maintainer : stegu
//     Lastmod : 20110822 (ijm)
//     License : Copyright (C) 2011 Ashima Arts. All rights reserved.
//               Distributed under the MIT License. See LICENSE file.
//               https://github.com/ashima/webgl-noise
//               https://github.com/stegu/webgl-noise

// This shader has been adapted for HLSL and ive implemented seeding 
// as well as adding my own helper functions

float2 mod289(float2 x) {
	return x - floor(x * (1.0 / 289.0)) * 289.0;
}

float3 mod289(float3 x) {
	return x - floor(x * (1.0 / 289.0)) * 289.0;
}

float4 mod289(float4 x) {
	return x - floor(x * (1.0 / 289.0)) * 289.0;
}

float3 permute(float3 x, uint seed) {
	return mod289(((x * 34.0) + seed) * x);
}

float4 permute(float4 x, uint seed) {
	return mod289(((x * 34.0) + seed) * x);
}

float4 taylorInvSqrt(float4 r) {
	return 1.79284291400159 - 0.85373472095314 * r; 
}

float simplex2d(uint seed, float2 v) {
	const float4 C = float4(0.211324865405187,	// (3.0-sqrt(3.0))/6.0
						0.366025403784439,		// 0.5*(sqrt(3.0)-1.0)
						-0.577350269189626,		// -1.0 + 2.0 * C.x
						0.024390243902439);		// 1.0 / 41.0

	// First corner
	float2 i = floor(v + dot(v, C.yy));
	float2 x0 = v - i + dot(i, C.xx);

	// Other corners
	float2 i1 = float2(step(x0.y, x0.x), step(x0.x, x0.y));
	float4 x12 = x0.xyxy + C.xxzz;
	x12.xy -= i1;

	// Permutations
	i = mod289(i);
	float3 p = permute(permute(
		i.y + float3(0.0, i1.y, 1.0), seed) +
		i.x + float3(0.0, i1.x, 1.0), seed);

	float3 m = max(0.5 - float3(dot(x0,x0), dot(x12.xy,x12.xy), dot(x12.zw,x12.zw)), 0.0);
	m = m * m;
	m = m * m;

	// Gradients: 41 points uniformly over a line, mapped onto a diamond.
	// The ring size 17*17 = 289 is close to a multiple of 41 (41*7 = 287)

	float3 x = 2.0 * frac(p * C.www) - 1.0;
	float3 h = abs(x) - 0.5;
	float3 ox = floor(x + 0.5);
	float3 a0 = x - ox;

	// Normalise gradients implicitly by scaling m
	// Approximation of: m *= inversesqrt( a0*a0 + h*h );
	m *= 1.79284291400159 - 0.85373472095314 * ( a0*a0 + h*h );

	// Compute final noise value at P
	float3 g;
	g.x = a0.x * x0.x + h.x * x0.y;
	g.yz = a0.yz * x12.xz + h.yz * x12.yw;
	return 130.0 * dot(m, g);
}

float simplex3d(uint seed, float3 v) {
	const float2 C = float2(1.0 / 6.0, 1.0 / 3.0);

	// First corner
	float3 i = floor(v + dot(v, C.yyy));
	float3 x0 = v - i + dot(i, C.xxx);

	// Other corners
	float3 g = step(x0.yzx, x0.xyz);
	float3 l = 1.0 - g;
	float3 i1 = min(g.xyz, l.zxy);
	float3 i2 = max(g.xyz, l.zxy);

	float3 x1 = x0 - i1 + C.xxx;
	float3 x2 = x0 - i2 + C.yyy;
	float3 x3 = x0 - 0.5;

	// Permutations
	i = mod289(i);
	float4 p = permute(permute(permute(
		i.z + float4(0.0, i1.z, i2.z, 1.0), seed) +
		i.y + float4(0.0, i1.y, i2.y, 1.0), seed) +
		i.x + float4(0.0, i1.x, i2.x, 1.0), seed);

	// Gradients: 7x7 points over a square, mapped onto an octahedron.
	// The ring size 17*17 = 289 is close to a multiple of 49 (49*6 = 294)
	float4 j = p - 49.0 * floor(p / 49.0);

	float4 x_ = floor(j / 7.0);
	float4 y_ = floor(j - 7.0 * x_);

	float4 x = (x_ * 2.0 + 0.5) / 7.0 - 1.0;
	float4 y = (y_ * 2.0 + 0.5) / 7.0 - 1.0;

	float4 h = 1.0 - abs(x) - abs(y);

	float4 b0 = float4(x.xy, y.xy);
	float4 b1 = float4(x.zw, y.zw);

	float4 s0 = floor(b0) * 2.0 + 1.0;
	float4 s1 = floor(b1) * 2.0 + 1.0;
	float4 sh = -step(h, 0.0);

	float4 a0 = b0.xzyw + s0.xzyw * sh.xxyy;
	float4 a1 = b1.xzyw + s1.xzyw * sh.zzww;

	float3 g0 = float3(a0.xy, h.x);
	float3 g1 = float3(a0.zw, h.y);
	float3 g2 = float3(a1.xy, h.z);
	float3 g3 = float3(a1.zw, h.w);

	// Normalise gradients
	float4 norm = taylorInvSqrt(float4(dot(g0, g0), dot(g1, g1), dot(g2, g2), dot(g3, g3)));
	g0 *= norm.x;
	g1 *= norm.y;
	g2 *= norm.z;
	g3 *= norm.w;

	// Mix final noise value
	float4 m = max(0.6 - float4(dot(x0, x0), dot(x1, x1), dot(x2, x2), dot(x3, x3)), 0.0);
	m = m * m;
	m = m * m;

	float4 px = float4(dot(x0, g0), dot(x1, g1), dot(x2, g2), dot(x3, g3));
	return 42.0 * dot(m, px);
}

float sampleSimplex(NoiseSettings settings, float2 coord) {
	float total = 0.0;
	float frequency = 1.0;
	float amplitude = 1.0;
	float maxValue = 0.0;
    
	float2 scaledPoint = coord * settings.scale;
	for (uint i = 0; i < settings.octaves; ++i) {
        total += simplex2d(settings.seed, scaledPoint * frequency) * amplitude;
		maxValue += amplitude;
		amplitude *= settings.persistence;
		frequency *= settings.lacunarity;
	}

	return total / maxValue;
}

float sampleSimplex(NoiseSettings settings, float3 coord) {
	float total = 0.0;
	float frequency = 1.0;
	float amplitude = 1.0;
	float maxValue = 0.0;
    
	float3 scaledPoint = coord * settings.scale;
	for (uint i = 0; i < settings.octaves; ++i) {
        total += simplex3d(settings.seed, scaledPoint * frequency) * amplitude;
		maxValue += amplitude;
		amplitude *= settings.persistence;
		frequency *= settings.lacunarity;
	}

	return total / maxValue;
}

float hash(uint seed, float3 p) {
	float3 seedOffset = float3(
		seed * 0.1031,
		seed * 0.11369,
		seed * 0.13787
	);
	p += seedOffset;

	return frac(sin(dot(p, float3(127.1, 311.7, 74.7))) * 43758.5453);
}

float3 randomPoint(uint seed, float3 cell, float3 jitter) {
	return cell + float3(
		hash(seed, cell),
		hash(seed, cell + 31.34),
		hash(seed, cell + 91.17)
	) * jitter;
}

// Worley Voronoi
float voronoi(uint seed, float3 p) {
	float3 cell = floor(p);
	float3 jitter = simplex3d(seed, p);
	float minDist = 99999;

	for (int x = -1; x <= 1; ++x) {
		for (int y = -1; y <= 1; ++y) {
			for (int z = -1; z <= 1; ++z) {
				float3 neighbour = cell + float3(x,y,z);
				float3 feature = randomPoint(seed, neighbour, jitter);
				float d = distance(p, feature);
				minDist = min(minDist, d);
			}
		}
	}

	return minDist;
}

float hash(uint seed, float2 p) {
    float3 p3 = frac(float3(p, seed) * float3(0.1031, 0.11369, 0.13782));
    p3 += dot(p3, p3.yzx + 19.19);
    return frac((p3.x + p3.y) * p3.z);
}

float voronoi(uint seed, float2 p) {
    float2 cell = floor(p);
    float2 fractional = p - cell;
    float minDist = 8.0;
    for (int x = -1; x <= 1; x++) {
        for (int y = -1; y <= 1; y++) {
            float2 neighbor = cell + float2(x, y);
            float2 pos = neighbor + hash(seed, neighbor);
            float d = distance(fractional, pos - cell);
            minDist = min(minDist, d);
        }
    }
    return minDist;
}