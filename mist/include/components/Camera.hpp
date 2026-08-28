#pragma once
#include <typeinfo>
#include "components/Transform.hpp"

namespace mist {
	class Camera {
	public:
		enum ProjectionType { Perspective = 0, Orthographic = 1 };
	public:
		Camera(Transform& transform);
		virtual ~Camera();

		bool IsEqual(const Camera& other) const;

		void RecreateCamera();

		glm::mat4 GetProjectionMatrix() const;
		glm::mat4 GetViewMatrix() const;
		glm::mat4 GetViewProjectionMatrix() const;
		
		ProjectionType GetProjectionType() const { return type; }
		void SetProjectionType(ProjectionType value) { type = value; RecreateCamera(); };

		void SetViewportSize(float width, float height);
		void SetNearPlane(float newNearPlane) { nearPlane = newNearPlane; RecreateCamera(); }
		void SetFarPlane(float newFarPlane) { farPlane = newFarPlane; RecreateCamera(); }

		float GetCameraWidth() const { return width; }
		float GetCameraHeight() const { return height; }
		float GetNearPlane() const { return nearPlane; }
		float GetFarPlane() const { return farPlane; }

		inline void SetTransform(Transform& value) { transformComponent = value; }
		inline Transform& GetTransform() const { return transformComponent; }

		// ORTHOGRAPHIC
		void SetOrthographicCamera(const float width, const float height, const float size = 10, const float nearPlane = -1, const float farPlane = 1);
		
		float GetOrthographicSize() const { return size; }
		void SetOrthographicSize(float value) { size = value; RecreateCamera(); }

		// PERSPECTIVE
		void SetPerspectiveCamera(const float width, const float height, const float fov = 60, const float nearPlane = 0.01f, const float farPlane = 1000);

		float GetPerspectiveFOV() const { return fov; }
		void SetPerspectiveFOV(float value) { fov = value; RecreateCamera(); }

		Camera(const Camera& other);				// Copy construct
		Camera& operator=(const Camera& other);	    // Copy assign

		bool operator==(const Camera& other) const { return typeid(*this) == typeid(other) && IsEqual(other); }
	private:
		// General camera
		ProjectionType type = ProjectionType::Perspective;
		glm::mat4 projectionMatrix = glm::mat4(0);
		Transform& transformComponent;

		float width = 1;
		float height = 1;
		float aspect = 1;
		float nearPlane = 0.1f;
		float farPlane = 0.1f;
		// Orthographic
		float size = 10;
		// Perspective
		float fov = 60;
	};

	class SceneCamera : public Camera {
	public:
		SceneCamera(Transform& transform);
	};
}