layout(std140, set = 0, binding = 3) uniform DirectionalLightData {
	vec3 LightDir;
	vec3 LightColor;
} directionalLightData;