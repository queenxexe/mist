#pragma once
#include "Math.hpp"

namespace mist {
	class Transform {
	public:
		Transform(glm::vec3 position = glm::vec3(0, 0, 0), glm::quat rotation = glm::quat_identity<float, glm::defaultp>(), glm::vec3 scale = glm::vec3(1, 1, 1));
		
		bool IsEqual(const Transform& other) const;

		void Rotate(float angle_in_radians, glm::vec3 axis);
		
		static glm::quat EulerToQuat(glm::vec3 rotation_in_degrees);
		static glm::vec3 QuatToEuler(glm::quat quaternion);
		
		glm::vec3 Left() const;
		glm::vec3 Right() const;
		glm::vec3 Up() const;
		glm::vec3 Down() const;
		glm::vec3 Forward() const;
		glm::vec3 Backward() const;
		
		glm::mat4 GetLocalToWorldMatrix() const;
		glm::mat4 GetWorldToLocalMatrix() const;
		
		glm::vec3 position;
		glm::quat rotation;
		glm::vec3 scale;
	};
}