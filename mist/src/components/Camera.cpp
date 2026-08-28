#include "components/Camera.hpp"
#include <Debug.hpp>

namespace mist {
	Camera::Camera(Transform& transform) : transformComponent(transform) {}

	Camera::~Camera() {}

	bool Camera::IsEqual(const Camera& other) const {
		return type == other.type &&
			projectionMatrix == other.projectionMatrix &&
			transformComponent.IsEqual(other.transformComponent) &&
			width == other.width &&
			height == other.height &&
			aspect == other.aspect &&
			size == other.size &&
			nearPlane == other.nearPlane &&
			farPlane == other.farPlane &&
			fov == other.fov;
	}

	void Camera::RecreateCamera() {
		MIST_ASSERT(width != 0, "Camera width is zero");
		MIST_ASSERT(height != 0, "Camera height is zero");
		MIST_ASSERT(nearPlane < farPlane, "Far plane is less than the near plane. Got them backwards?");

		if (type == Orthographic) {
			float left = -size * aspect * 0.5f;
			float right = size * aspect * 0.5f;
			float bottom = -size * 0.5f;
			float top = size * 0.5f;
			
			projectionMatrix = glm::orthoLH_ZO(left, right, bottom, top, nearPlane, farPlane);
		} else {
			projectionMatrix = glm::perspectiveLH_ZO(glm::radians(fov), aspect, nearPlane, farPlane);
		}
	}

	glm::mat4 Camera::GetProjectionMatrix() const {
		return projectionMatrix;
	}

	glm::mat4 Camera::GetViewMatrix() const {
		return glm::lookAtLH(
			transformComponent.position, 
			transformComponent.position + transformComponent.Forward(),
			transformComponent.Up()
		);
	}

	glm::mat4 Camera::GetViewProjectionMatrix() const {
		return projectionMatrix * GetViewMatrix();
	}

	void Camera::SetViewportSize(float _width, float _height) {
		width = _width;
		height = _height;
		aspect = (float)_width / (float)_height;
		RecreateCamera();
	}

	void Camera::SetPerspectiveCamera(const float _width, const float _height, const float _fov, const float _nearPlane, const float _farPlane) {
		width = _width;
		height = _height;
		aspect = width / height;
		fov = _fov;
		nearPlane = _nearPlane;
		farPlane = _farPlane;
		type = Perspective;

		RecreateCamera();
	}

	Camera::Camera(const Camera& other) : 
		type(other.type), 
		projectionMatrix(other.projectionMatrix), 
		transformComponent(other.transformComponent), 
		width(other.width), 
		height(other.height), 
		aspect(other.aspect),
		size(other.size), 
		nearPlane(other.nearPlane), 
		farPlane(other.farPlane),
		fov(other.fov) {}

	Camera& Camera::operator=(const Camera& other) {
		if (this == &other)
			return *this;

		type = other.type;
		projectionMatrix = other.projectionMatrix;
		transformComponent = other.transformComponent;
		width = other.width;
		height = other.height;
		aspect = other.aspect;
		size = other.size;
		nearPlane = other.nearPlane;
		farPlane = other.farPlane;
		fov = other.fov;

		return *this;
	}

	void Camera::SetOrthographicCamera(const float _width, const float _height, const float _size, const float _nearPlane, const float _farPlane) {
		width = _width;
		height = _height;
		aspect = width / height;
		size = _size;
		nearPlane = _nearPlane;
		farPlane = _farPlane;
		type = Orthographic;

		RecreateCamera();
	}

	SceneCamera::SceneCamera(Transform& transform) : Camera(transform) {}
}
