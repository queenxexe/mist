#pragma once
#ifndef GLM_ENABLE_EXPERIMENTAL
	#define GLM_ENABLE_EXPERIMENTAL
#endif
#ifndef GLM_FORCE_RADIANS
	#define GLM_FORCE_RADIANS
#endif
#ifndef GLM_FORCE_DEPTH_ZERO_TO_ONE
	#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#endif
#ifndef GLM_FORCE_LEFT_HANDED
	#define GLM_FORCE_LEFT_HANDED
#endif

#include <glm/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/matrix_decompose.hpp>

namespace mist {
	class Math {
	public:
		static bool AllEqual(const glm::ivec3 a, const glm::ivec3 b);
		static bool AllGreaterOrEqual(const glm::ivec3 a, const glm::ivec3 b);
		static bool AllLessOrEqual(const glm::ivec3 a, const glm::ivec3 b);

		static bool AnyEqual(const glm::ivec3 a, const glm::ivec3 b);
		static bool AnyLess(const glm::ivec3 vec, const uint32_t value);
		static bool AnyGreaterOrEqual(const glm::ivec3 vec, const uint32_t value);

		static uint32_t DistanceSq(const glm::ivec3 a, const glm::ivec3 b);
	};
}