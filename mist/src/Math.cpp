#include "Math.hpp"

namespace mist {
	bool Math::AllEqual(const glm::ivec3 a, const glm::ivec3 b) {
		return a.x == b.x && a.y == b.y && a.z == b.z;
	}

	bool Math::AllGreaterOrEqual(const glm::ivec3 a, const glm::ivec3 b) {
		return a.x >= b.x && a.y >= b.y && a.z >= b.z;
	}

	bool Math::AllLessOrEqual(const glm::ivec3 a, const glm::ivec3 b) {
		return a.x <= b.x && a.y <= b.y && a.z <= b.z;
	}

	bool Math::AnyEqual(const glm::ivec3 a, const glm::ivec3 b) {
		return a.x == b.x || a.y == b.y || a.z == b.z;
	}

	bool Math::AnyLess(const glm::ivec3 vec, const uint32_t value) {
		return vec.x < value || vec.y < value || vec.z < value; 
	}

	bool Math::AnyGreaterOrEqual(const glm::ivec3 vec, const uint32_t value) {
		return vec.x >= value || vec.y >= value || vec.z >= value;
	}

	uint32_t Math::DistanceSq(const glm::ivec3 a, const glm::ivec3 b) {
		glm::ivec3 d = a - b;
		// Cant do glm::dot() here as it doesnt accept ivec3 as a parameter
		return
			d.x * d.x +
			d.y * d.y +
			d.z * d.z;
	}
}