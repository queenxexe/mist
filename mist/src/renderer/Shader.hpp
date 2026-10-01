#pragma once
#include <string>
#include <unordered_map>
#include "renderer/RenderTypes.hpp"

namespace mist {
	class Shader {
	public:
		virtual ~Shader() {}

		virtual void Cleanup() = 0;

		virtual void Bind(const uint8_t renderDataId) const = 0;
		virtual void Unbind(const uint8_t renderDataId) const = 0;

		virtual void SetPushConstant(const uint8_t renderDataId, const std::string& name, const int size, const void* value) = 0;

		virtual const std::string& GetName() const = 0;
		virtual const CullMode GetCullMode() const = 0;
		virtual const bool IsDepthTesting() const = 0;
	};
}