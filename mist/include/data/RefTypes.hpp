#pragma once
#include <stdint.h>
#define INVALID_RESOURCE_ID UINT32_MAX

// Handles that applications will use to interact with the engine
// and works well with ECS components
namespace mist {
	typedef uint32_t ResourceID;

	struct ResourceHandle {
	public:
		ResourceID id;
	};

	struct MaterialRef : public ResourceHandle {};
	struct ShaderRef : public ResourceHandle {};
	struct ImageRef : public ResourceHandle {};
}
