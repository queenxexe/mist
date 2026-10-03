layout(std140, set = 0, binding = 0) uniform CameraData {
	uniform mat4 ViewProjectionMatrix;
	uniform vec3 CameraPosition;
} cameraData;
