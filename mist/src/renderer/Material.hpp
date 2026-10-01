#pragma once
#include <memory>
#include <data/RefTypes.hpp>
#include <data/Image.hpp>

namespace mist {
	enum class MaterialParameterType {
		UniformBuffer,
		Texture
	};

	struct MaterialParameter {
		MaterialParameterType type;
		std::vector<std::byte> data;
		ImageRef texture;
	};

	// This is an instance of a materials data and is where data is set to
	class Material {
	public:
		Material(const ShaderRef& shader) : shader(shader) {}
		virtual ~Material() {};

		Material(const Material& other) = delete;
		Material& operator=(const Material& other) = delete;
		
		inline const ShaderRef& GetShaderRef() const { return shader; } 
		
		virtual void Cleanup() = 0;
		
		virtual void Bind(const uint8_t renderDataID) = 0;

		virtual void SetTexture(const uint8_t renderDataID, const std::string& name, const std::shared_ptr<Image>& texture) = 0;
		virtual void SetUniformData(const uint8_t renderDataID, const std::string& name, size_t size, const void* value) = 0;
		virtual void SetPushConstant(const uint8_t renderDataID, const std::string& name, size_t size, const void* value) = 0;
	protected:
		ShaderRef shader;
	};
}